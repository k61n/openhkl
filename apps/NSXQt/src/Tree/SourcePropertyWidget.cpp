#include "include/Tree/SourcePropertyWidget.h"
#include "ui_SourcePropertyWidget.h"
#include "include/Tree/SourceItem.h"
#include "Diffractometer.h"
#include "Source.h"
#include "Units.h"

SourcePropertyWidget::SourcePropertyWidget(SourceItem* caller,QWidget *parent) :
    _caller(caller),
    QWidget(parent),
    ui(new Ui::SourcePropertyWidget)
{
    ui->setupUi(this);
    auto source=_caller->getExperiment()->getDiffractometer()->getSource();
    ui->doubleSpinBox_Wavelength->setValue(source->getWavelength());
    ui->doubleSpinBox_Height->setValue(source->getHeight()/SX::Units::mm);
    ui->doubleSpinBox_Width->setValue(source->getWidth()/SX::Units::mm);
}

SourcePropertyWidget::~SourcePropertyWidget()
{
    delete ui;
}

void SourcePropertyWidget::on_doubleSpinBox_Wavelength_valueChanged(double arg1)
{
     auto source=_caller->getExperiment()->getDiffractometer()->getSource();
     source->setWavelength(arg1);
}

void SourcePropertyWidget::on_doubleSpinBox_Width_valueChanged(double arg1)
{
     auto source=_caller->getExperiment()->getDiffractometer()->getSource();
     source->setWidth(arg1*SX::Units::mm);
}

void SourcePropertyWidget::on_doubleSpinBox_Height_valueChanged(double arg1)
{
     auto source=_caller->getExperiment()->getDiffractometer()->getSource();
     source->setHeight(arg1*SX::Units::mm);
}
