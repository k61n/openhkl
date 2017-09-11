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
    _minimizer()
{
    ui->setupUi(this);

    // Remove row numbers in table
    ui->tableWidget_Sample->verticalHeader()->setVisible(false);
    ui->tableWidget_Detector->verticalHeader()->setVisible(false);

    ui->tableWidget_Sample->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_Detector->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidget_Sample->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->tableWidget_Detector->setEditTriggers(QAbstractItemView::AllEditTriggers);

    // Fill up the UB matrix parameters with the current value
    setLatticeParams();

    // Fill up the wavelength with the current value
    setWavelength();

    // Set up the minimizer
    setMinimizer();

    // Fill up the sample and detector offsets tables
    createOffsetsTables();

    ui->labelalpha->setText(QString(QChar(0x03B1)));
    ui->labelbeta->setText(QString(QChar(0x03B2)));
    ui->labelgamma->setText(QString(QChar(0x03B3)));

    // Connect checkbox for wavelength refinement
    connect(ui->checkBox_Wavelength,&QCheckBox::toggled,[&](bool checked){_solution.refineSource(checked);});
    connect(ui->pushButton_Refine,SIGNAL(clicked()),this,SLOT(refineParameters()));
    connect(ui->pushButton_Reset,SIGNAL(clicked()),this,SLOT(resetParameters()));
}

DialogRefineUnitCell::~DialogRefineUnitCell()
{
    delete ui;
}

void DialogRefineUnitCell::setMinimizer()
{
    auto diffractometer = _experiment->getDiffractometer();
    auto detector = diffractometer->getDetector();
    auto sample = diffractometer->getSample();
    auto source = diffractometer->getSource();

    // Set the UB minimizer with parameters
    _solution.setDetector(detector);
    _solution.setSample(sample);
    _solution.setSource(source);

    int start=10;

    auto& mono = source->getSelectedMonochromator();
    //_solution.refineParameter(9,!mono.isOffsetFixed());

    int nSampleOffsets = sample->hasGonio() ? sample->getGonio()->getNAxes() : 0;
    for (int i = 0; i < nSampleOffsets; ++i) {
        auto axis=sample->getGonio()->getAxis(i);
        //_solution.refineSample(i,!axis->hasOffsetFixed());
        _solution.setValue(start+i,axis->getOffset());
    }

    start += nSampleOffsets;
    int nDetectorOffsets = detector->hasGonio() ? detector->getGonio()->getNAxes() : 0;
    for (int i = 0; i < nDetectorOffsets; ++i) {
        auto axis=detector->getGonio()->getAxis(i);
        //_solution.refineDetector(i,!axis->hasOffsetFixed());
        _solution.setValue(start+i,axis->getOffset());
    }
}

void DialogRefineUnitCell::setLatticeParams()
{
    nsx::CellCharacter ch = _unitCell->character();
    ui->doubleSpinBoxa->setValue(ch.a);
    ui->doubleSpinBoxb->setValue(ch.b);
    ui->doubleSpinBoxc->setValue(ch.c);
    ui->doubleSpinBoxalpha->setValue(ch.alpha/nsx::deg);
    ui->doubleSpinBoxbeta->setValue(ch.beta/nsx::deg);
    ui->doubleSpinBoxgamma->setValue(ch.gamma/nsx::deg);
}

void DialogRefineUnitCell::setWavelength()
{
    auto& mono = _experiment->getDiffractometer()->getSource()->getSelectedMonochromator();
    ui->doubleSpinBox_Wavelength->setValue(mono.getWavelength());
}

void DialogRefineUnitCell::setSampleOffsets()
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
        connect(item3,&QCheckBox::toggled, [this, i](bool checked){_solution.refineSample(i, checked);});
        ui->tableWidget_Sample->setCellWidget(i,3,item3);
    }
}

void DialogRefineUnitCell::setDetectorOffsets()
{
    // Get the number of axis for the sample
    auto sample = _experiment->getDiffractometer()->getSample();
    int nAxesSample = sample->hasGonio() ? sample->getGonio()->getNAxes() : 0;

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
        //Connect checkbox to fixing parameters
        connect(item3,&QCheckBox::toggled, [this,i](bool checked){_solution.refineDetector(i, checked);});
    }
}

void DialogRefineUnitCell::setSolution(const nsx::UBSolution& solution)
{
    // Get the sample
    auto sample = _experiment->getDiffractometer()->getSample();
    int nAxesSample = sample->hasGonio() ? sample->getGonio()->getNAxes() : 0;

    auto&& sampleOffsets = solution.sampleOffsets();
    auto&& sigmaSampleOffsets = solution.sigmaSampleOffsets();
    auto&& detectorOffsets = solution.detectorOffsets();
    auto&& sigmaDetectorOffsets = solution.sigmaDetectorOffsets();

    for (int i = 0; i < nAxesSample; ++i) {
        ui->tableWidget_Sample->item(i,1)->setData(Qt::EditRole, sampleOffsets[i]);
        ui->tableWidget_Sample->item(i,2)->setData(Qt::EditRole, sigmaSampleOffsets[i]);
    }

    // Get the detector
    auto detector = _experiment->getDiffractometer()->getDetector();
    int nAxesDet = detector->hasGonio() ? detector->getGonio()->getNAxes() : 0;

    for (int i = 0; i < nAxesDet; ++i) {
        ui->tableWidget_Detector->item(i,1)->setData(Qt::EditRole, detectorOffsets[i]);
        ui->tableWidget_Detector->item(i,2)->setData(Qt::EditRole, sigmaDetectorOffsets[i]);
    }
    setLatticeParams();
}

void DialogRefineUnitCell::cellSampleHasChanged(int i, int j)
{
    // A new offset has been entered
    if (j == 1) {
        auto axis = _experiment->getDiffractometer()->getSample()->getGonio()->getAxis(i);
        //bool offsetFixed = axis->hasOffsetFixed();
        //axis->setOffsetFixed(false);
        double value = ui->tableWidget_Sample->item(i,j)->data(Qt::EditRole).toDouble();
        axis->setOffset(value);
        //axis->setOffsetFixed(offsetFixed);
        _solution.setValue(10+i,value);
        //_solution.refineSample(i);
    }
}

void DialogRefineUnitCell::cellDetectorHasChanged(int i, int j)
{
    // A new offset has been entered
    if (j==1) {
        auto axis=_experiment->getDiffractometer()->getDetector()->getGonio()->getAxis(i);
        //bool offsetFixed=axis->hasOffsetFixed();
        //axis->setOffsetFixed(false);
        double value=ui->tableWidget_Detector->item(i,j)->data(Qt::EditRole).toDouble();
        axis->setOffset(value);
        //axis->setOffsetFixed(offsetFixed);

        // Get the sample
        auto sample=_experiment->getDiffractometer()->getSample();
        int nAxesSample = sample->hasGonio() ? sample->getGonio()->getNAxes() : 0;
        _solution.setValue(10+nAxesSample+i,value);
        //_solution.refine
    }
}

void DialogRefineUnitCell::refineParameters()
{
    int nhits = 0;
    for (auto&& peak: _peaks) {
        Eigen::RowVector3d hkl;
        bool indexingSuccess = peak->getMillerIndices(*_unitCell,hkl,true);
        if (indexingSuccess && peak->isSelected() && !peak->isMasked()) {
            _minimizer.addPeak(*peak,hkl);
            ++nhits;
        }
    }

    std::ostringstream os;
    os<<nhits<<" peaks considered for UB-refinement";
    ui->textEdit_Solution->setText(QString::fromStdString(os.str()));
    os.str("");

    //auto M=_unitCell->reciprocalBasis();
    _solution.setCell(_unitCell);

    int test = _minimizer.run(_solution, 100);
    if (test != 1) {
        ui->textEdit_Solution->setTextColor(QColor("red"));
        ui->textEdit_Solution->setText("No solution found within convergence criteria.");
        return; // why not change ?
    }

    const auto& solution=_minimizer.solution();
    os << solution;

    _unitCell->setReciprocalBasis(solution.ub());

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
    setSolution(solution);
}

void DialogRefineUnitCell::createOffsetsTables()
{
    setSampleOffsets();
    setDetectorOffsets();
    connect(ui->tableWidget_Sample,SIGNAL(cellChanged(int,int)),this,SLOT(cellSampleHasChanged(int,int)));
    connect(ui->tableWidget_Detector,SIGNAL(cellChanged(int,int)),this,SLOT(cellDetectorHasChanged(int,int)));
}

void DialogRefineUnitCell::resetParameters()
{
    // Get the sample
    auto sampleAxes=_experiment->getDiffractometer()->getSample()->getGonio()->getAxes();
    for (auto a : sampleAxes) {
        a->setOffset(0.00);
    }
    setSampleOffsets();
    ui->tableWidget_Sample->update();

    // Get the detector
    auto detectorAxes=_experiment->getDiffractometer()->getDetector()->getGonio()->getAxes();
    for (auto a : detectorAxes) {
        a->setOffset(0.00);
    }
    setDetectorOffsets();
    ui->tableWidget_Detector->update();
}
