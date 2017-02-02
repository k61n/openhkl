#include "dialogs/DialogAutoIndexing.h"
#include "ui_DialogAutoIndexing.h"

#include <map>
#include <string>

#include <QMenu>

#include <Eigen/Dense>

#include <QItemSelectionModel>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QtDebug>

#include "Experiment.h"
#include <nsxlib/crystal/FFTIndexing.h>
#include <nsxlib/crystal/GruberReduction.h>
#include "IData.h"
#include <nsxlib/crystal/NiggliReduction.h>
#include <nsxlib/crystal/UBMinimizer.h>
#include "models/CollectedPeaksModel.h"
#include "models/CollectedPeaksDelegate.h"

using SX::Crystal::tVector;
using SX::Crystal::BravaisType;
using SX::Crystal::LatticeCentring;
using SX::Crystal::FFTIndexing;
using SX::Crystal::NiggliReduction;
using SX::Crystal::UBMinimizer;
using SX::Crystal::UBSolution;
using SX::Data::IData;
using SX::Units::deg;

DialogAutoIndexing::DialogAutoIndexing(std::shared_ptr<Experiment> experiment, std::vector<sptrPeak3D> peaks, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogAutoIndexing),
    _experiment(experiment),
    _peaks(peaks)
{
    ui->setupUi(this);

    setModal(true);

    _unitCells = _experiment->getDiffractometer()->getSample()->getUnitCells();
    for (auto uc : _unitCells)
        ui->unitCells->addItem(QString::fromStdString(uc->getName()));

    connect(ui->index,SIGNAL(clicked()),this,SLOT(autoIndex()));

    // Accept solution and set Unit-Cell
    connect(ui->solutions->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectSolution(int)));
}

DialogAutoIndexing::~DialogAutoIndexing()
{
    delete ui;
}

void DialogAutoIndexing::autoIndex()
{
    int nPeaks = _peaks.size();
    // Check that a minimum number of peaks have been selected for indexing
    if (nPeaks < 10)
    {
        QMessageBox::warning(this, tr("NSXTool"),tr("Need at least 10 peaks for autoindexing"));
        return;
    }

    if (_experiment == nullptr)
        return;

    if (ui->unitCells->count() == 0)
    {
        QMessageBox::warning(this, tr("NSXTool"),tr("No unit cells defined for autoindexing"));
        return;
    }

    sptrUnitCell selectedUnitCell = _unitCells[ui->unitCells->currentIndex()];

    // Clear the current solution list
    _solutions.clear();

    // Store Q vectors at rest
    std::vector<Eigen::Vector3d> qvects;
    qvects.reserve(nPeaks);
    for (auto peak : _peaks)
    {
        if (peak->isSelected() && !peak->isMasked())
            qvects.push_back(peak->getQ());
    }

    qDebug() << "Searching direct lattice vectors using" << nPeaks << "peaks defined on numors:";

    // Set up a FFT indexer object
    FFTIndexing indexing(5,ui->maxCellDim->value());

    int nSolutions = ui->maxNumSolutions->value();
    int nStacks = ui->nStacks->value();
    // Find the best tvectors
    std::vector<tVector> tvects=indexing.findOnSphere(qvects, nStacks, nSolutions);

    auto source = _experiment->getDiffractometer()->getSource();
    auto detector = _experiment->getDiffractometer()->getDetector();
    auto sample = _experiment->getDiffractometer()->getSample();

    std::vector<std::pair<UnitCell,double>> newSolutions;
    newSolutions.reserve(nSolutions*nSolutions*nSolutions);

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
                    newSolutions.push_back(std::make_pair(cell, 0.0));
                }
            }
        }
    }

    qDebug() << "" << newSolutions.size()<< " possible solutions found";
    qDebug() << "Refining solutions";

    //#pragma omp parallel for
    for (int idx = 0; idx < newSolutions.size(); ++idx) {

        UBMinimizer minimizer;
        minimizer.setSample(sample);
        minimizer.setDetector(detector);
        minimizer.setSource(source);

        UnitCell cell = newSolutions[idx].first;
        cell.setName(selectedUnitCell->getName());
        cell.setHKLTolerance(selectedUnitCell->getHKLTolerance());

        // Only the UB matrix parameters are used for fit
        int nParameters= 10 + sample->getNAxes() + detector->getNAxes();
        for (int i = 9; i < nParameters; ++i)
            minimizer.refineParameter(i,false);

        int success = 0;
        for (auto peak : _peaks)
        {
            Eigen::RowVector3d hkl;
            bool indexingSuccess = peak->getMillerIndices(cell,hkl,true);
            if (indexingSuccess && peak->isSelected() && !peak->isMasked())
            {
                minimizer.addPeak(*peak,hkl);
                ++success;
            }
        }

        // The number of peaks must be at least for a proper minimization
        if (success < 10)
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

            cell.setName(selectedUnitCell->getName());
            cell.setHKLTolerance(selectedUnitCell->getHKLTolerance());

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
            for (auto peak : _peaks)
            {
                if (peak->isSelected() && !peak->isMasked())
                {
                    maxscore++;
                    Eigen::RowVector3d hkl;
                    bool indexingSuccess = peak->getMillerIndices(cell,hkl,true);
                    if (indexingSuccess)
                        score++;
                }
            }
            // Percentage of indexing
            score /= 0.01*maxscore;
            newSolutions[idx].first = cell;
            newSolutions[idx].second = score;
        }
        minimizer.resetParameters();
    }

    _solutions.reserve(newSolutions.size());

    // remove the false solutions
    for (auto&& it = newSolutions.begin(); it != newSolutions.end(); ++it)
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

void DialogAutoIndexing::selectSolution(int index)
{
    *_unitCells[ui->unitCells->currentIndex()] = _solutions[index].first;

    QString solutionNumber = QString::number(index+1);
    QString selectedUnitCellName = ui->unitCells->currentText();
    QMessageBox::information(this, tr("NSXTool"),tr("Solution %1 set to %2 unit cell").arg(solutionNumber,selectedUnitCellName));

    emit cellUpdated(_unitCells[ui->unitCells->currentIndex()]);
}
