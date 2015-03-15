#include <set>

#include <QtDebug>

#include "IData.h"
#include "UnitCell.h"
#include "Units.h"

#include "ui_UnitCellPropertyWidget.h"
#include "Logger.h"
#include "Tree/UnitCellPropertyWidget.h"
#include "Tree/UnitCellItem.h"
#include "DialogFindUnitCell.h"

UnitCellPropertyWidget::UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent) :
    _unitCellItem(caller),
    QWidget(parent),
    ui(new Ui::UnitCellPropertyWidget)
{
    ui->setupUi(this);
    connect(ui->doubleSpinBoxa,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxb,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxc,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxalpha,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxbeta,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxgamma,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));

    getLatticeParams();
}

UnitCellPropertyWidget::~UnitCellPropertyWidget()
{
    delete ui;
}

void UnitCellPropertyWidget::setLatticeParams()
{
    double a=ui->doubleSpinBoxa->value();
    double b=ui->doubleSpinBoxb->value();
    double c=ui->doubleSpinBoxc->value();
    double alpha=ui->doubleSpinBoxalpha->value();
    double beta=ui->doubleSpinBoxbeta->value();
    double gamma=ui->doubleSpinBoxgamma->value();

    try
    {
    _unitCellItem->getCell()->setParams(a,b,c,alpha*SX::Units::deg,beta*SX::Units::deg,gamma*SX::Units::deg);
    }catch(...)
    {

    }

}

void UnitCellPropertyWidget::on_pushButton_Info_clicked()
{
   qDebug() << "" << *(_unitCellItem->getCell());
}

void UnitCellPropertyWidget::on_pushButton_Index_clicked()
{
    emit activateIndexingMode(_unitCellItem->getCell());
}

void UnitCellPropertyWidget::on_pushButton_AutoIndexing_clicked()
{
    DialogFindUnitCell* dialog=new DialogFindUnitCell(_unitCellItem->getExperiment(),this);
    // Ensure that lattice parameters are updated if a solution is accepted
    connect(dialog,SIGNAL(solutionAccepted(SX::Crystal::UnitCell)),this,SLOT(setCell(SX::Crystal::UnitCell)));
    dialog->exec();
}

void UnitCellPropertyWidget::setCell(const SX::Crystal::UnitCell& cell)
{
    *(_unitCellItem->getCell().get())=cell;
    getLatticeParams();
}

void UnitCellPropertyWidget::getLatticeParams()
{
    auto sample=_unitCellItem->getCell();
    ui->doubleSpinBoxa->setValue(sample->getA());
    ui->doubleSpinBoxb->setValue(sample->getB());
    ui->doubleSpinBoxc->setValue(sample->getC());
    ui->doubleSpinBoxalpha->setValue(sample->getAlpha()/SX::Units::deg);
    ui->doubleSpinBoxbeta->setValue(sample->getBeta()/SX::Units::deg);
    ui->doubleSpinBoxgamma->setValue(sample->getGamma()/SX::Units::deg);
}
