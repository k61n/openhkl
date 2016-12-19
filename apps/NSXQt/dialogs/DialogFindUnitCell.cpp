#include "DialogFindUnitCell.h"
#include "ui_FindUnitCellDialog.h"
#include "Experiment.h"
#include "Peak3D.h"
#include "IData.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QtDebug>
#include "UnitCell.h"
#include "UBMinimizer.h"
#include "NiggliReduction.h"
#include "GruberReduction.h"
#include "FFTIndexing.h"
#include "Units.h"

#include <stdexcept>
#include <mutex>

using SX::Crystal::UnitCell;
using SX::Crystal::UBMinimizer;
using SX::Crystal::UBSolution;
using SX::Crystal::NiggliReduction;
using SX::Crystal::GruberReduction;
using SX::Units::deg;

DialogFindUnitCell::DialogFindUnitCell(std::shared_ptr<SX::Instrument::Experiment> experiment, QWidget *parent):
    QDialog(parent),
     ui(new Ui::DialogFindUnitCell),
    _experiment(experiment)
{
    ui->setupUi(this);
    setPeaks();

    // Display I/SIgmaI as slider move
    connect(ui->horizontalSlider_NumberOfPeaks,SIGNAL(valueChanged(int)),this,SLOT(setIOverSigma(int)));

    // Make sure that the user can not edit the content of the table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Selection of a cellin the table select the whole line.
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->label_amax->setText("Maximum cell dimension ("+QString((QChar) 0x0212b)+")");

    // Accept solution and set Unit-Cell
    connect(ui->tableView->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectSolution(int)));
}

DialogFindUnitCell::~DialogFindUnitCell()
{
    delete ui;
}

void DialogFindUnitCell::setIOverSigma(int peaknumber)
{
    double maxIsI=_peaks[peaknumber-1]->getIOverSigmaI();
    ui->lineEdit_IOverSigmaI->setText("I/"+QString((QChar) 0x03C3)+"I > "+QString::number(maxIsI));
}

void DialogFindUnitCell::setPeaks()
{
    // Collect peaks from all IData belonging to this experiment
    const auto& datamap=_experiment->getData();
    for (const auto& data: datamap)
    {
        auto& peaks=data.second->getPeaks();
        std::copy(peaks.begin(),peaks.end(),std::back_inserter(_peaks));
    }
    // Sort peaks by decreasing I/SigmaI
    std::sort(_peaks.begin(),
              _peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
                {
                    return (p1->getIOverSigmaI()>p2->getIOverSigmaI());
                }
            );
    // Populate menus
    int npeaks=_peaks.size();
    ui->horizontalSlider_NumberOfPeaks->setMinimum(1);
    ui->horizontalSlider_NumberOfPeaks->setMaximum(npeaks);
    ui->horizontalSlider_NumberOfPeaks->setValue(npeaks);
    ui->spinBox_NumberOfPeaks->setValue(npeaks);
    if (npeaks)
    {
        setIOverSigma(npeaks);
    }
}

void DialogFindUnitCell::on_pushButton_SearchUnitCells_clicked()
{
    int npeaks = ui->horizontalSlider_NumberOfPeaks->value();
    // Need at leat 10 peaks
    if (npeaks < 10) {
        QMessageBox::warning(this, tr("NSXTool"),tr("Need at least 10 peaks for autoindexing"));
        return;
    }

    // clear the table
    _solutions.clear();
    buildSolutionsTable();

    // Store Q vectors at rest
    std::vector<Eigen::Vector3d> qvects;
    qvects.reserve(npeaks);

    for (auto& peak: _peaks)
        if (peak->isSelected() && !peak->isMasked())
            qvects.push_back(peak->getQ());

    qDebug() << "Searching direct lattice vectors using" << npeaks << "peaks defined on numors:";

    // Set up a FFT indexer object
    SX::Crystal::FFTIndexing indexing(5,ui->doubleSpinBox_amax->value());
    indexing.addVectors(qvects);

    int nSolutions = ui->spinBox_nSolutions->value();
    int numStacks = ui->stacksSpinBox->value();
    std::vector<SX::Crystal::tVector> tvects = indexing.findOnSphere(numStacks, nSolutions);

    auto source = _experiment->getDiffractometer()->getSource();
    auto detector = _experiment->getDiffractometer()->getDetector();
    auto sample = _experiment->getDiffractometer()->getSample();

    // jmf debugging
    qDebug() << "size of tvects " << tvects.size();

    std::vector<std::pair<SX::Crystal::UnitCell,double>> new_solutions;
    new_solutions.reserve(nSolutions*nSolutions*nSolutions);

    for (int i = 0; i < tvects.size(); ++i) {
        for (int j = i+1; j < tvects.size(); ++j) {
            for (int k = j+1; k < tvects.size(); ++k) {
                Eigen::Vector3d& v1 = tvects[i]._vect;
                Eigen::Vector3d& v2 = tvects[j]._vect;
                Eigen::Vector3d& v3 = tvects[k]._vect;

                volatile double det = v1.dot(v2.cross(v3));

                // change orientation to right-handed
                if (det < 0) {
                    v1 *= -1, v2 *= -1, v3 *= -1;
                }

                if (std::abs(det) > 20.0) {
                    UnitCell cell = UnitCell::fromDirectVectors(v1, v2, v3);
                    new_solutions.push_back(std::make_pair(cell, 0.0));
                }
            }
        }
    }

    qDebug() << "" << new_solutions.size()<< " possible solutions found";
    qDebug() << "Refining solutions and diffractometers offsets";

    //#pragma omp parallel for
    for (unsigned int idx = 0; idx < new_solutions.size(); ++idx) {

        UBMinimizer minimizer;
        minimizer.setSample(sample);
        minimizer.setDetector(detector);
        minimizer.setSource(source);

        SX::Crystal::UnitCell cell = new_solutions[idx].first;

        // Only the UB matrix parameters are used for fit
        int nParameters= 10 + sample->getNAxes() + detector->getNAxes();
        for (int i = 9; i < nParameters; ++i)
            minimizer.refineParameter(i, false);

        int success = 0;
        for (auto peak: _peaks) {
            if (peak->hasIntegerHKL(cell, 0.2) && peak->isSelected() && !peak->isMasked()) {
                minimizer.addPeak(*peak);
                ++success;
            }
        }

        if (success < 10)
            continue;

        Eigen::Matrix3d M = cell.getReciprocalStandardM();
        minimizer.setStartingUBMatrix(M);
        int ret = minimizer.runGSL(100);

        if (ret == 1) {
            UBSolution sln = minimizer.getSolution();
            try {
                cell = SX::Crystal::UnitCell::fromReciprocalVectors(sln._ub.row(0),sln._ub.row(1),sln._ub.row(2));
                cell.setReciprocalCovariance(sln._covub);

            } catch(std::exception& e) {
                //qDebug() << e.what();
                continue;
            }
            double tolerance = ui->niggliSpinBox->value();
            NiggliReduction niggli(cell.getMetricTensor(), tolerance);
            Eigen::Matrix3d newg, P;
            niggli.reduce(newg, P);
            cell.transform(P);

            // use GruberReduction::reduce to get Bravais type
            tolerance = ui->gruberSpinBox->value();
            GruberReduction gruber(cell.getMetricTensor(), tolerance);
            SX::Crystal::LatticeCentring c;
            SX::Crystal::BravaisType b;

            try {
                gruber.reduce(P,c,b);
                cell.setLatticeCentring(c);
                cell.setBravaisType(b);
            }
            catch(std::exception& e) {
                qDebug() << "Gruber reduction error:" << e.what()
                         << ". Consider changing the Gruber tolerance parameter";
                continue;
            }

            if (!ui->checkBox_NiggliOnly->isChecked()) {
                cell.transform(P);
            }

            double score = 0.0;
            double maxscore = 0.0;

            for (auto peak : _peaks) {
                if (peak->isSelected() && !peak->isMasked()) {
                    maxscore++;
                    if (peak->hasIntegerHKL(cell, 0.2))
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

    qDebug() << "Done refining solutions, building table...";
    buildSolutionsTable();
}

void DialogFindUnitCell::buildSolutionsTable()
{
    // Sort solutions by decreasing quality.
    // For equal quality, smallest volume is first
    typedef std::pair<SX::Crystal::UnitCell,double> Soluce;
    std::sort(_solutions.begin(),_solutions.end(),
              [](const Soluce& s1, const Soluce& s2) -> bool
    {
        if ( std::abs(s1.second-s2.second) < 1e-8)
            return (s1.first.getVolume()<s2.first.getVolume());
        else
            return (s1.second>s2.second);
    }
    );
    // Create table with 9 columns
    QStandardItemModel* model = new QStandardItemModel(_solutions.size(), 9, this);
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
   ui->tableView->setModel(model);
}

void DialogFindUnitCell::selectSolution(int i)
{
    QMessageBox::information(this, tr("NSXTool"),
                          tr("Solution set"));
    emit solutionAccepted(_solutions[i].first);
}
