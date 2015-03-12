#include "include/UnitCellPropertyWidget.h"
#include "ui_UnitCellPropertyWidget.h"
#include "UnitCellItem.h"
#include <iostream>
#include "UnitCell.h"
#include "Units.h"
#include <QtDebug>
#include "Logger.h"
#include <set>
#include "IData.h"

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

    auto sample=caller->getCell();
    ui->doubleSpinBoxa->setValue(sample->getA());
    ui->doubleSpinBoxb->setValue(sample->getB());
    ui->doubleSpinBoxc->setValue(sample->getC());
    ui->doubleSpinBoxalpha->setValue(sample->getAlpha()/SX::Units::deg);
    ui->doubleSpinBoxbeta->setValue(sample->getBeta()/SX::Units::deg);
    ui->doubleSpinBoxgamma->setValue(sample->getGamma()/SX::Units::deg);

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

void UnitCellPropertyWidget::on_pushButton_Index_2_pressed()
{
    auto datamap=_unitCellItem->getExperiment()->getData();
    std::vector<SX::Crystal::Peak3D*> allpeaks;
    for (auto data: datamap)
    {
        auto peaks=data.second->getPeaks();
        std::copy(peaks.begin(),peaks.end(),std::back_inserter(allpeaks));
    }
    auto cellptr=_unitCellItem->getCell();
}
