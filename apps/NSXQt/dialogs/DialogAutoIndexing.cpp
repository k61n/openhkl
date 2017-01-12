#include "dialogs/DialogAutoIndexing.h"
#include "ui_DialogAutoIndexing.h"

#include <map>
#include <string>

#include <Eigen/Dense>

#include <QMessageBox>
#include <QStandardItemModel>
#include <QtDebug>

#include "FFTIndexing.h"
#include "GruberReduction.h"
#include "IData.h"
#include "NiggliReduction.h"
#include "UBMinimizer.h"
#include "models/CollectedPeaksModel.h"

using SX::Crystal::tVector;
using SX::Crystal::BravaisType;
using SX::Crystal::LatticeCentring;
using SX::Crystal::FFTIndexing;
using SX::Crystal::NiggliReduction;
using SX::Crystal::UBMinimizer;
using SX::Crystal::UBSolution;

DialogAutoIndexing::DialogAutoIndexing(std::shared_ptr<Experiment> experiment, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogAutoIndexing),
    _experiment(experiment)
{
    ui->setupUi(this);

    std::map<std::string,std::shared_ptr<IData>>  datamap=_experiment->getData();
    std::vector<std::shared_ptr<SX::Data::IData>> datav;

    auto func = [&](std::pair<std::string,std::shared_ptr<SX::Data::IData>> value){datav.push_back(value.second);};

    std::for_each(datamap.begin(), datamap.end(), func);

    CollectedPeaksModel *model = new CollectedPeaksModel();
    model->setPeaks(datav);
    model->setUnitCells(_experiment->getDiffractometer()->getSample()->getUnitCells());
    ui->collectedPeaks->setModel(model);

    _peaks = model->getPeaks();

    connect(ui->index,SIGNAL(clicked()),this,SLOT(autoIndex()));

}

DialogAutoIndexing::~DialogAutoIndexing()
{
    delete ui;
}

void DialogAutoIndexing::autoIndex()
{
    // Check that a minimum number of peaks have been selected for indexing
    QModelIndexList selectedPeaks = ui->collectedPeaks->selectionModel()->selectedRows();
    int nPeaks = selectedPeaks.count();
    if (nPeaks < 10)
    {
        QMessageBox::warning(this, tr("NSXTool"),tr("Need at least 10 peaks for autoindexing"));
        return;
    }

    // Clear the current solution list
    _solutions.clear();

    // Store Q vectors at rest
    std::vector<Eigen::Vector3d> qvects;
    qvects.reserve(nPeaks);
    for (const auto& index : selectedPeaks)
    {
        auto peak=_peaks[index.row()];
        qDebug()<<index.row()<<index.column();
        if (peak->isSelected() && !peak->isMasked())
            qvects.push_back(peak->getQ());
    }

    qDebug() << "Searching direct lattice vectors using" << nPeaks << "peaks defined on numors:";

    // Set up a FFT indexer object
    FFTIndexing indexing(5,ui->maxCellDim->value());
    indexing.addVectors(qvects);

    int nSolutions = ui->maxNumSolutions->value();
    // Find the best tvectors
    std::vector<tVector> tvects=indexing.findOnSphere(30, nSolutions);

    auto source = _experiment->getDiffractometer()->getSource();
    auto detector = _experiment->getDiffractometer()->getDetector();
    auto sample = _experiment->getDiffractometer()->getSample();

    std::vector<std::pair<UnitCell,double>> new_solutions;
    new_solutions.reserve(nSolutions*nSolutions*nSolutions);

    for (int i = 0; i < nSolutions; ++i)
    {
        for (int j = i+1; j < nSolutions; ++j)
        {
            for (int k = j+1; k < nSolutions; ++k)
            {
                Eigen::Vector3d& v1=tvects[i]._vect;
                Eigen::Vector3d& v2=tvects[j]._vect;
                Eigen::Vector3d& v3=tvects[k]._vect;

                if (v1.dot(v2.cross(v3)) > 20.0)
                {
                    UnitCell cell = UnitCell::fromDirectVectors(v1, v2, v3);
                    new_solutions.push_back(std::make_pair(cell, 0.0));
                }
            }
        }
    }

    qDebug() << "" << new_solutions.size()<< " possible solutions found";
    qDebug() << "Refining solutions and diffractometers offsets";

    //#pragma omp parallel for
    for (int idx = 0; idx < new_solutions.size(); ++idx) {

        UBMinimizer minimizer;
        minimizer.setSample(sample);
        minimizer.setDetector(detector);
        minimizer.setSource(source);

        UnitCell cell = new_solutions[idx].first;

        // Only the UB matrix parameters are used for fit
        int nParameters= 10 + sample->getNAxes() + detector->getNAxes();
        for (int i = 9; i < nParameters; ++i)
            minimizer.refineParameter(i,false);

        double hklTol = ui->hklTolerance->value();
        int maxNumSolutions = ui->maxNumSolutions->value();
        int success = 0;
        for (const auto& index : selectedPeaks)
        {
            auto peak = _peaks[index.row()];
            if (peak->hasIntegerHKL(cell,hklTol) && peak->isSelected() && !peak->isMasked())
            {
                minimizer.addPeak(*peak);
                ++success;
            }
        }

        if (success < maxNumSolutions)
            continue;

        Eigen::Matrix3d M = cell.getReciprocalStandardM();
        minimizer.setStartingUBMatrix(M);
        int ret = minimizer.runGSL(100);

        if (ret == 1)
        {
            UBSolution sln = minimizer.getSolution();
            try
            {
                cell = UnitCell::fromReciprocalVectors(sln._ub.row(0),sln._ub.row(1),sln._ub.row(2));
                cell.setReciprocalCovariance(sln._covub);

            }
            catch(std::exception& e)
            {
                continue;
            }
            double tolerance = ui->niggliTolerance->value();
            NiggliReduction niggli(cell.getMetricTensor(), tolerance);
            Eigen::Matrix3d newg, P;
            niggli.reduce(newg, P);
            cell.transform(P);

            // use GruberReduction::reduce to get Bravais type
            tolerance = ui->gruberTolerance->value();
            GruberReduction gruber(cell.getMetricTensor(), tolerance);
            LatticeCentring c;
            BravaisType b;

            try
            {
                gruber.reduce(P,c,b);
                cell.setLatticeCentring(c);
                cell.setBravaisType(b);
            }
            catch(std::exception& e) {
                //qDebug() << "Gruber reduction error:" << e.what();
                //continue;
            }

            if (!ui->niggliReduction->isChecked())
                cell.transform(P);

            double score=0.0;
            double maxscore=0.0;
            for (const auto& index : selectedPeaks)
            {
                auto peak = _peaks[index.row()];
                if (peak->isSelected() && !peak->isMasked())
                {
                    maxscore++;
                    if (peak->hasIntegerHKL(cell,hklTol))
                        score++;
                }
            }
            // Percentage of indexing
            score /= 0.01*maxscore;
            new_solutions[idx].first = cell;
            new_solutions[idx].second = score;
        }
        minimizer.resetParameters();
    }

    _solutions.reserve(new_solutions.size());

    // remove the false solutions
    for (auto&& it = new_solutions.begin(); it != new_solutions.end(); ++it)
        if (it->second > 0.1)
            _solutions.push_back(*it);

    _solutions.shrink_to_fit();

    qDebug() << "Done refining solutions, building solutions table.";
    buildSolutionsTable();

}

void DialogAutoIndexing::buildSolutionsTable()
{
    // Sort solutions by decreasing quality.
    // For equal quality, smallest volume is first
    typedef std::pair<UnitCell,double> Soluce;
    std::sort(_solutions.begin(),_solutions.end(),[](const Soluce& s1, const Soluce& s2) -> bool
    {
        if (s1.second==s2.second)
            return (s1.first.getVolume()<s2.first.getVolume());
        else
            return (s1.second>s2.second);
    }
    );

    // Create table with 9 columns
    QStandardItemModel* model=new QStandardItemModel(_solutions.size(),9,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("a"));
    model->setHorizontalHeaderItem(1,new QStandardItem("b"));
    model->setHorizontalHeaderItem(2,new QStandardItem("c"));
    model->setHorizontalHeaderItem(3,new QStandardItem(QString((QChar) 0x03B1)));
    model->setHorizontalHeaderItem(4,new QStandardItem(QString((QChar) 0x03B2)));
    model->setHorizontalHeaderItem(5,new QStandardItem(QString((QChar) 0x03B3)));
    model->setHorizontalHeaderItem(6,new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7,new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(8,new QStandardItem("Quality"));
    // Display solutions
    for (unsigned int i=0;i<_solutions.size();++i)
    {
        auto& cell=_solutions[i].first;
        double quality=_solutions[i].second;
        double a,b,c,alpha,beta,gamma;
        double sa,sb,sc,salpha,sbeta,sgamma;
        cell.getParameters(a,b,c,alpha,beta,gamma);
        cell.getParametersSigmas(sa,sb,sc,salpha,sbeta,sgamma);
        QStandardItem* col1=new QStandardItem(QString::number(a,'f',3) + "("+ QString::number(sa*1000,'f',0)+")");
        QStandardItem* col2=new QStandardItem(QString::number(b,'f',3) + "("+ QString::number(sb*1000,'f',0)+")");
        QStandardItem* col3=new QStandardItem(QString::number(c,'f',3) + "("+ QString::number(sc*1000,'f',0)+")");
        QStandardItem* col4=new QStandardItem(QString::number(alpha/deg,'f',3)+ "("+ QString::number(salpha/deg*1000,'f',0)+")");
        QStandardItem* col5=new QStandardItem(QString::number(beta/deg,'f',3)+"("+ QString::number(sbeta/deg*1000,'f',0)+")");
        QStandardItem* col6=new QStandardItem(QString::number(gamma/deg,'f',3)+ "("+ QString::number(sgamma/deg*1000,'f',0)+")");
        QStandardItem* col7=new QStandardItem(QString::number(cell.getVolume(),'f',3));
        QStandardItem* col8=new QStandardItem(QString::fromStdString(cell.getBravaisTypeSymbol()));
        QStandardItem* col9=new QStandardItem(QString::number(quality,'f',2)+"%");
        model->setItem(i,0,col1);
        model->setItem(i,1,col2);
        model->setItem(i,2,col3);
        model->setItem(i,3,col4);
        model->setItem(i,4,col5);
        model->setItem(i,5,col6);
        model->setItem(i,6,col7);
        model->setItem(i,7,col8);
        model->setItem(i,8,col9);
    }
    ui->solutions->setModel(model);
}

