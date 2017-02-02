#include "SourcePropertyWidget.h"
#include "ui_SourcePropertyWidget.h"
#include <QDebug>

#include "SourceItem.h"

#include "Diffractometer.h"
#include "Monochromator.h"
#include "Source.h"
#include <nsxlib/utils/Units.h>

SourcePropertyWidget::SourcePropertyWidget(SourceItem* caller,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourcePropertyWidget),
    _caller(caller)
{
    ui->setupUi(this);
    std::shared_ptr<Source> source=_caller->getExperiment()->getDiffractometer()->getSource();
    auto monos = source->getMonochromators();
    for (auto&& m : monos)
        ui->comboBox_Monochromators->addItem(QString::fromStdString(m.getName()));

    try {
        ui->doubleSpinBox_Wavelength->setValue(source->getSelectedMonochromator()->getWavelength());
        ui->doubleSpinBox_FWHM->setValue(source->getSelectedMonochromator()->getFWHM());
        ui->doubleSpinBox_Height->setValue(source->getSelectedMonochromator()->getHeight()/SX::Units::mm);
        ui->doubleSpinBox_Width->setValue(source->getSelectedMonochromator()->getWidth()/SX::Units::mm);
    }
    catch (std::exception& e) {
        qCritical() << e.what();
    }
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
     source->getSelectedMonochromator()->setWidth(arg1*SX::Units::mm);
}

void SourcePropertyWidget::on_doubleSpinBox_Height_valueChanged(double arg1)
{
     auto source=_caller->getExperiment()->getDiffractometer()->getSource();
     source->getSelectedMonochromator()->setHeight(arg1*SX::Units::mm);
}

void SourcePropertyWidget::on_comboBox_Monochromators_currentIndexChanged(int index)
{
    auto source=_caller->getExperiment()->getDiffractometer()->getSource();
    source->setSelectedMonochromator(index);
    ui->doubleSpinBox_Wavelength->setValue(source->getSelectedMonochromator()->getWavelength());
    ui->doubleSpinBox_FWHM->setValue(source->getSelectedMonochromator()->getFWHM());
    ui->doubleSpinBox_Height->setValue(source->getSelectedMonochromator()->getHeight()/SX::Units::mm);
    ui->doubleSpinBox_Width->setValue(source->getSelectedMonochromator()->getWidth()/SX::Units::mm);
}
