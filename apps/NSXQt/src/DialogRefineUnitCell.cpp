#include "include/DialogRefineUnitCell.h"
#include "ui_DialogRefineUnitCell.h"
#include "Experiment.h"
#include "Diffractometer.h"
#include "Sample.h"
#include "Detector.h"
#include "Source.h"
#include "Gonio.h"
#include "Axis.h"
#include "Units.h"
#include "IData.h"

DialogRefineUnitCell::DialogRefineUnitCell(SX::Instrument::Experiment* experiment,std::shared_ptr<SX::Crystal::UnitCell> cell,QWidget *parent) :
    _experiment(experiment),
    _cell(cell),
    QDialog(parent),
    ui(new Ui::DialogRefineUnitCell),
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

    // Set the UB minimizer with parameters
    _minimizer.setDetector(_experiment->getDiffractometer()->getDetector());
    _minimizer.setSource(_experiment->getDiffractometer()->getSource());
    _minimizer.setSample(_experiment->getDiffractometer()->getSample());

    createTable();

    // Connect checkbox for wavelength refinement
    connect(ui->checkBox_Wavelength,&QCheckBox::toggled,[=](bool checked){refineParameter(checked,9);});

    getLatticeParams();
    getWavelength();

    for (int i=9;i<=17;++i)
        _minimizer.refineParameter(i,false);

}

DialogRefineUnitCell::~DialogRefineUnitCell()
{
    delete ui;
}
void DialogRefineUnitCell::getLatticeParams()
{
    ui->doubleSpinBoxa->setValue(_cell->getA());
    ui->doubleSpinBoxb->setValue(_cell->getB());
    ui->doubleSpinBoxc->setValue(_cell->getC());
    ui->doubleSpinBoxalpha->setValue(_cell->getAlpha()/SX::Units::deg);
    ui->doubleSpinBoxbeta->setValue(_cell->getBeta()/SX::Units::deg);
    ui->doubleSpinBoxgamma->setValue(_cell->getGamma()/SX::Units::deg);
}

void DialogRefineUnitCell::getWavelength()
{
    auto source=_experiment->getDiffractometer()->getSource();
    ui->doubleSpinBox_Wavelength->setValue(source->getWavelength());
}

void DialogRefineUnitCell::refineParameter(bool checked, int i)
{
	_minimizer.refineParameter(i,checked);
}
void DialogRefineUnitCell::cellSampleHasChanged(int i, int j)
{
    if (j==1) // new offset has been entered
    {
        auto axis=_experiment->getDiffractometer()->getSample()->getGonio()->getAxis(i);
        bool offsetFixed=axis->hasOffsetFixed();
        axis->setOffsetFixed(false);
        axis->setOffset(ui->tableWidget_Sample->item(i,j)->data(Qt::EditRole).toDouble());
        axis->setOffsetFixed(offsetFixed);
    }
}

void DialogRefineUnitCell::cellDetectoreHasChanged(int i, int j)
{
    if (j==1) // new offset has been entered
    {
        auto axis=_experiment->getDiffractometer()->getDetector()->getGonio()->getAxis(i);
        bool offsetFixed=axis->hasOffsetFixed();
        axis->setOffsetFixed(false);
        axis->setOffset(ui->tableWidget_Detector->item(i,j)->data(Qt::EditRole).toDouble());
        axis->setOffsetFixed(offsetFixed);
    }
}

void DialogRefineUnitCell::on_pushButton_Refine_clicked()
{
    int nhits=0;
    const auto& mapdata=_experiment->getData();
    for (auto data: mapdata)
    {
        const auto& peaks=data.second->getPeaks();
        for (auto peak: peaks)
        {
            if (peak->hasIntegerHKL(*(_cell.get())) && !peak->isMasked() && peak->isSelected())
            {
                _minimizer.addPeak(*peak);
                nhits++;
            }
        }
    }
    qDebug() << nhits << " peaks considered for UB-refinement";


    auto M=_cell->getReciprocalStandardM();
    _minimizer.setStartingUBMatrix(M);

    int test=_minimizer.run(100);
    if (test!=1)
        return;

    std::cout <<_minimizer.getSolution() << std::endl;
    auto solution=_minimizer.getSolution();
    _cell->setReciprocalVectors(solution._ub.row(0),solution._ub.row(1),solution._ub.row(2));
    createTable();
}

void DialogRefineUnitCell::createTable()
{

    getLatticeParams();

    //Get the sample, iterate over axis
    auto sample=_experiment->getDiffractometer()->getSample();
    int naxesSample=sample->getNAxes();
    ui->tableWidget_Sample->setRowCount(naxesSample);
    ui->tableWidget_Sample->setColumnCount(3);
    for (int i=0;i<naxesSample;++i)
    {
        auto axis=sample->getGonio()->getAxis(i);
        QTableWidgetItem* item0=new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->getLabel().c_str()));
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        if (axis->isPhysical())
            item0->setBackgroundColor(QColor("#FFDDDD"));
        else
            item0->setBackgroundColor(QColor("#DDFFDD"));
        QTableWidgetItem* item1=new QTableWidgetItem();
        item1->setData(Qt::EditRole, double(axis->getOffset()));
        item1->setFlags(item1->flags() | Qt::ItemIsEditable);
        ui->tableWidget_Sample->setItem(i,0,item0);
        ui->tableWidget_Sample->setItem(i,1,item1);
        QCheckBox* check=new QCheckBox(this);
        check->setChecked(!axis->hasOffsetFixed());
        // Connect checkbox to fixing this parameter
        connect(check,&QCheckBox::toggled,[=](bool checked){refineParameter(checked,10+i);});
        ui->tableWidget_Sample->setCellWidget(i,2,check);

    }
    connect(ui->tableWidget_Sample,SIGNAL(cellChanged(int,int)),this,SLOT(cellSampleHasChanged(int,int)));
    // Get the detector, iterate over axis
    auto detector=_experiment->getDiffractometer()->getDetector();
    int naxesDet=detector->getNAxes();
    ui->tableWidget_Detector->setRowCount(naxesDet);
    ui->tableWidget_Detector->setColumnCount(3);
    for (int i=0;i<naxesDet;++i)
    {
        auto axis=detector->getGonio()->getAxis(i);
        QTableWidgetItem* item0=new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->getLabel().c_str()));
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        if (axis->isPhysical())
            item0->setBackgroundColor(QColor("#FFDDDD"));
        else
            item0->setBackgroundColor(QColor("#DDFFDD"));
        QTableWidgetItem* item1=new QTableWidgetItem();
        item1->setData(Qt::EditRole, double(axis->getOffset()));
        item1->setFlags(item1->flags() | Qt::ItemIsEditable);
        ui->tableWidget_Detector->setItem(i,0,item0);
        ui->tableWidget_Detector->setItem(i,1,item1);
        QCheckBox* check=new QCheckBox(this);
        //Connect checkbox to fixing parameters
        connect(check,&QCheckBox::toggled,[=](bool checked){refineParameter(checked,10+i+naxesSample);});
        ui->tableWidget_Detector->setCellWidget(i,2,check);
    }

    connect(ui->tableWidget_Detector,SIGNAL(cellChanged(int,int)),this,SLOT(cellDetectoreHasChanged(int,int)));
}
