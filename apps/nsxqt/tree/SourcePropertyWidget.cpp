#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Monochromator.h>
#include <nsxlib/instrument/InstrumentTypes.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/logger/Logger.h>
#include <nsxlib/utils/Units.h>

#include "models/SourceItem.h"
#include "SourcePropertyWidget.h"

#include "ui_SourcePropertyWidget.h"

SourcePropertyWidget::SourcePropertyWidget(SourceItem* caller,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourcePropertyWidget),
    _caller(caller)
{
    ui->setupUi(this);
    nsx::sptrSource source=_caller->getExperiment()->getDiffractometer()->getSource();
    auto monos = source->getMonochromators();
    for (auto&& m : monos) {
        ui->comboBox_Monochromators->addItem(QString::fromStdString(m.getName()));
    }

    auto& mono=source->getSelectedMonochromator();

    try {
        ui->doubleSpinBox_Wavelength->setValue(mono.getWavelength());
        ui->doubleSpinBox_FWHM->setValue(mono.getFWHM());
        ui->doubleSpinBox_Height->setValue(mono.getHeight()/nsx::mm);
        ui->doubleSpinBox_Width->setValue(mono.getWidth()/nsx::mm);
    }
    catch (std::exception& e) {
        nsx::error() << e.what();
    }
}

SourcePropertyWidget::~SourcePropertyWidget()
{
    delete ui;
}

void SourcePropertyWidget::on_doubleSpinBox_Wavelength_valueChanged(double arg1)
{
     auto& mono=_caller->getExperiment()->getDiffractometer()->getSource()->getSelectedMonochromator();
     mono.setWavelength(arg1);
}

void SourcePropertyWidget::on_doubleSpinBox_Width_valueChanged(double arg1)
{
    auto& mono=_caller->getExperiment()->getDiffractometer()->getSource()->getSelectedMonochromator();
    mono.setWidth(arg1*nsx::mm);
}

void SourcePropertyWidget::on_doubleSpinBox_Height_valueChanged(double arg1)
{
    auto& mono=_caller->getExperiment()->getDiffractometer()->getSource()->getSelectedMonochromator();
    mono.setHeight(arg1*nsx::mm);
}

void SourcePropertyWidget::on_comboBox_Monochromators_currentIndexChanged(int index)
{
    auto source=_caller->getExperiment()->getDiffractometer()->getSource();
    source->setSelectedMonochromator(index);
    auto& mono=source->getSelectedMonochromator();
    ui->doubleSpinBox_Wavelength->setValue(mono.getWavelength());
    ui->doubleSpinBox_FWHM->setValue(mono.getFWHM());
    ui->doubleSpinBox_Height->setValue(mono.getHeight()/nsx::mm);
    ui->doubleSpinBox_Width->setValue(mono.getWidth()/nsx::mm);
}
