#include <sstream>

#include <QCheckBox>
#include <QLayout>
#include <QStatusBar>

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Axis.h>
#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/instrument/Monochromator.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/mathematics/Minimizer.h>
#include <nsxlib/utils/Units.h>

#include "DialogRefineUnitCell.h"
#include "DoubleTableItemDelegate.h"

#include "ui_DialogRefineUnitCell.h"

DialogRefineUnitCell::DialogRefineUnitCell(nsx::sptrExperiment experiment,
                                           nsx::sptrUnitCell unitCell,
                                           nsx::PeakList peaks,
                                           QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogRefineUnitCell),
    _experiment(std::move(experiment)),
    _unitCell(std::move(unitCell)),
    _peaks(std::move(peaks)),
    _initialValues(nullptr, nullptr, nullptr, unitCell),
    _currentValues(nullptr, nullptr, nullptr, unitCell)
{
    ui->setupUi(this);

    // Remove row numbers in table
    ui->tableWidget_Sample->verticalHeader()->setVisible(false);
    ui->tableWidget_Detector->verticalHeader()->setVisible(false);

    ui->tableWidget_Sample->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_Detector->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidget_Sample->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->tableWidget_Detector->setEditTriggers(QAbstractItemView::AllEditTriggers);

    // set up the QCustomPlot object
    ui->plotWidget->addGraph();
    ui->plotWidget->addGraph();
    ui->plotWidget->addGraph();
    
    auto diffractometer = _experiment->getDiffractometer();
    auto detector = diffractometer->getDetector();
    auto sample = diffractometer->getSample();
    auto source = diffractometer->getSource();

    // get starting values of UB and offsets
    _initialValues = nsx::UBSolution(source, sample, detector, _unitCell);
    _currentValues = _initialValues;

    // Fill up the sample and detector offsets tables
    createOffsetTables();
    updateParameters();

    // alpha, beta, gamma are special characters
    ui->labelalpha->setText(QString(QChar(0x03B1)));
    ui->labelbeta->setText(QString(QChar(0x03B2)));
    ui->labelgamma->setText(QString(QChar(0x03B3)));

    connect(ui->pushButton_Refine, SIGNAL(clicked()), this, SLOT(refineParameters()));
    connect(ui->pushButton_Reset, SIGNAL(clicked()), this, SLOT(resetParameters()));
}

DialogRefineUnitCell::~DialogRefineUnitCell()
{
    delete ui;
}

void DialogRefineUnitCell::updateParameters()
{
    // update UC parameters
    nsx::CellCharacter ch = _unitCell->character();
    ui->doubleSpinBoxa->setValue(ch.a);
    ui->doubleSpinBoxb->setValue(ch.b);
    ui->doubleSpinBoxc->setValue(ch.c);
    ui->doubleSpinBoxalpha->setValue(ch.alpha/nsx::deg);
    ui->doubleSpinBoxbeta->setValue(ch.beta/nsx::deg);
    ui->doubleSpinBoxgamma->setValue(ch.gamma/nsx::deg);

    // update wavelength
    auto& mono = _experiment->getDiffractometer()->getSource()->getSelectedMonochromator();
    ui->doubleSpinBox_Wavelength->setValue(mono.getWavelength());

    // Fill the table
    for (int i = 0; i < _currentValues._sampleOffset.size(); ++i) {
        // Set the first column of the table: the axis name
        auto item1 = ui->tableWidget_Sample->item(i, 1);
        auto item2 = ui->tableWidget_Sample->item(i, 2);

        // offset values
        item1->setData(Qt::EditRole, _currentValues._sampleOffset(i));

        // the axis offsets errors
        item2->setData(Qt::EditRole, _currentValues._sigmaSample(i));      
    }
    
    // Fill the table
    for (int i = 0; i < _currentValues._detectorOffset.size(); ++i) {
        // Set the first column of the table: the axis name
        auto item1 = ui->tableWidget_Detector->item(i, 1);
        auto item2 = ui->tableWidget_Detector->item(i, 2);
    
        // offset values
        item1->setData(Qt::EditRole, _currentValues._detectorOffset(i));
    
        // the axis offsets errors
        item2->setData(Qt::EditRole, _currentValues._sigmaDetector(i));      
    }

    // update the error plot
    updatePlot();
}

void DialogRefineUnitCell::refineParameters()
{
    nsx::UBMinimizer minimizer(_currentValues);

    // determine which parameters to refine
    if (ui->checkBox_Wavelength) {
        minimizer.refineSource(true);
    }

    // sample parameters
    for (int i = 0; i < _currentValues._sampleOffset.size(); ++i) {
        auto item3 = dynamic_cast<QCheckBox*>(ui->tableWidget_Sample->cellWidget(i, 3));
        assert(item3 != nullptr);
        minimizer.refineSample(i, item3->isChecked());
    }

    // detector parameters
    for (int i = 0; i < _currentValues._detectorOffset.size(); ++i) {
        auto item3 = dynamic_cast<QCheckBox*>(ui->tableWidget_Detector->cellWidget(i, 3));
        assert(item3 != nullptr);
        minimizer.refineDetector(i, item3->isChecked());
    }

    // peaks to be used in fit
    int nhits = 0;
    for (auto&& peak: _peaks) {
        Eigen::RowVector3d hkl;
        bool indexingSuccess = peak->getMillerIndices(*_unitCell,hkl,true);
        if (indexingSuccess && peak->isSelected() && !peak->isMasked()) {
            minimizer.addPeak(*peak,hkl);
            ++nhits;
        }
    }

    std::ostringstream os;
    os<<nhits<<" peaks considered for UB-refinement";
    ui->textEdit_Solution->setText(QString::fromStdString(os.str()));
    os.str("");

    int test = minimizer.run(100);
    if (test != 1) {
        ui->textEdit_Solution->setTextColor(QColor("red"));
        ui->textEdit_Solution->setText("No solution found within convergence criteria.");
        return; // why not change ?
    }

    _currentValues = minimizer.solution();
    _currentValues.apply();
    os << _currentValues;

    // calculate the new quality of the fit
    unsigned int total = 0, count = 0;
    for (auto&& peak: _peaks) {
        if(!peak->isMasked() && peak->isSelected()) {
            ++total;
            Eigen::RowVector3d hkl;
            bool indexingSuccess = peak->getMillerIndices(*_unitCell,hkl,true);
            if (indexingSuccess) {
                ++count;
            }
        }
    }
    os << "Quality: " << double(count) * 100.0 / double(total);

    // update textbox with output
    ui->textEdit_Solution->append(QString::fromStdString(os.str()));
    ui->textEdit_Solution->append(QString::fromStdString(os.str()));

    // update the displayed paramters in the GUI
    updateParameters();
}

void DialogRefineUnitCell::resetParameters()
{
    _initialValues._sourceOffset = 0.0;
    _initialValues._sampleOffset.setZero();
    _initialValues._detectorOffset.setZero();
    _initialValues.apply();
    _currentValues = _initialValues;
    ui->textEdit_Solution->append("\nResetting parameters to initial values.\n");
    updateParameters();
}

void DialogRefineUnitCell::createOffsetTables()
{
    //Get the sample, iterate over axis
    auto sample = _experiment->getDiffractometer()->getSample();
    int nAxesSample = sample->hasGonio() ? sample->getGonio()->getNAxes() : 0;

    // Set the number of row corresponding to the number of offsets/axis
    ui->tableWidget_Sample->setRowCount(nAxesSample);

    // Set the number of columns (axis name, axis offet, axis offset error, refinable state)
    ui->tableWidget_Sample->setColumnCount(4);

    // Allow double to be entered with a better precision
    ui->tableWidget_Sample->setItemDelegateForColumn(1,new DoubleTableItemDelegate(ui->tableWidget_Sample));

    // Fill the table
    for (int i = 0; i < nAxesSample; ++i) {
        // Set the first column of the table: the axis name
        auto axis = sample->getGonio()->getAxis(i);
        auto item0 = new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->getLabel().c_str()));
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);

        if (axis->isPhysical()) {
            item0->setBackgroundColor(QColor("#FFDDDD"));
        } else {
            item0->setBackgroundColor(QColor("#DDFFDD"));
        }
        ui->tableWidget_Sample->setItem(i,0,item0);

        // Set the second column of the table: the axis offsets
        auto item1 = new QTableWidgetItem();
        item1->setData(Qt::EditRole, double(axis->getOffset()));
        item1->setFlags(item1->flags() | Qt::ItemIsEditable);
        ui->tableWidget_Sample->setItem(i, 1, item1);

        // Set the third column of the table: the axis offsets errors
        auto item2 = new QTableWidgetItem();
        item2->setData(Qt::EditRole, 0.000);
        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_Sample->setItem(i, 2, item2);

        // Set the fourth column of the table: the axis refinable state
        auto item3 = new QCheckBox(this);
        //item3->setChecked(!axis->hasOffsetFixed());
        item3->setChecked(false);
        // Connect checkbox to fixing this parameter
        ui->tableWidget_Sample->setCellWidget(i,3,item3);
    }

    // Get the detector
    auto detector = _experiment->getDiffractometer()->getDetector();
    int nAxesDet = detector->hasGonio() ? detector->getGonio()->getNAxes() : 0;

    // Set the number of row corresponding to the number of offsets/axis
    ui->tableWidget_Detector->setRowCount(nAxesDet);

    // Set the number of columns (axis name, axis offet, axis offset error, refinable state)
    ui->tableWidget_Detector->setColumnCount(4);

    // Allow double to be entered with a better precision
    ui->tableWidget_Detector->setItemDelegateForColumn(1,new DoubleTableItemDelegate(ui->tableWidget_Detector));

    for (int i = 0; i < nAxesDet; ++i) {
        // Set the first column of the table: the axis name
        auto axis = detector->getGonio()->getAxis(i);
        auto item0 = new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->getLabel().c_str()));
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        // Set two different colors for the first column color according to to the axis type (physical or not)
        if (axis->isPhysical()) {
            item0->setBackgroundColor(QColor("#FFDDDD"));
        } else {
            item0->setBackgroundColor(QColor("#DDFFDD"));
        }
        ui->tableWidget_Detector->setItem(i,0,item0);

        // Set the second column of the table: the axis offsets
        auto item1 = new QTableWidgetItem();
        item1->setData(Qt::EditRole, axis->getOffset());
        item1->setFlags(item1->flags() | Qt::ItemIsEditable);
        ui->tableWidget_Detector->setItem(i, 1, item1);

        // Set the third column of the table: the axis offsets errors
        auto item2 = new QTableWidgetItem();
        item2->setData(Qt::EditRole, 0.000);
        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_Detector->setItem(i, 2, item2);

        // Set the fourth column of the table: the axis refinable state
        auto item3 = new QCheckBox(this);
        //item3->setChecked(!axis->hasOffsetFixed());
        item3->setChecked(false);
        ui->tableWidget_Detector->setCellWidget(i, 3, item3);
    }   
}

void DialogRefineUnitCell::updatePlot()
{
    QVector<double> frames;
    QVector<double> d[3];
    QPen pens[3] = { QPen(QColor(255, 0, 0)), QPen(QColor(0, 255, 0)), QPen(QColor(0, 0, 255))};
    double ymin, ymax;
    double fmin, fmax;

    fmin = 1e100;
    fmax = -1e100;
    ymin = 1e100;
    ymax = -1e100;

    for (auto peak: _peaks) {
        double f = peak->getShape().center()[2];
        Eigen::RowVector3d q = peak->getQ();
        Eigen::RowVector3d hkl, dhkl;
        fmin = std::min(f, fmin);
        fmax = std::max(f, fmax);

        if (peak->getMillerIndices(hkl)) {
            dhkl = hkl - q*peak->getActiveUnitCell()->basis();
            frames.push_back(f);

            for (auto i = 0; i < 3; ++i) {
                d[i].push_back(dhkl(i));
                ymin = std::min(ymin, dhkl(i));
                ymax = std::max(ymax, dhkl(i));
            }
        }
    }

    for (auto i = 0; i < 3; ++i) {
        ui->plotWidget->graph(i)->setData(frames, d[i]);
        ui->plotWidget->graph(i)->setPen(pens[i]);
        ui->plotWidget->graph(i)->setLineStyle(QCPGraph::lsNone);
        ui->plotWidget->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
    }

    ui->plotWidget->xAxis->setLabel("frame number");
    ui->plotWidget->yAxis->setLabel("index error");
    ui->plotWidget->xAxis->setRange(fmin, fmax);
    ui->plotWidget->yAxis->setRange(ymin, ymax);
}
