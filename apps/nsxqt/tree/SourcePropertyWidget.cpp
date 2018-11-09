#include <nsxlib/Diffractometer.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Monochromator.h>
#include <nsxlib/Source.h>
#include <nsxlib/Units.h>

#include "SourceItem.h"
#include "SourcePropertyWidget.h"

#include "ui_SourcePropertyWidget.h"

SourcePropertyWidget::SourcePropertyWidget(SourceItem* caller,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourcePropertyWidget),
    _caller(caller)
{
    ui->setupUi(this);

    const auto &source =_caller->experiment()->diffractometer()->source();

    const auto &monos = source.monochromators();

    for (auto&& m : monos) {
        ui->comboBox_Monochromators->addItem(QString::fromStdString(m.name()));
    }

    const auto& mono = source.selectedMonochromator();

    try {
        ui->doubleSpinBox_Wavelength->setValue(mono.wavelength());
        ui->doubleSpinBox_FWHM->setValue(mono.fullWidthHalfMaximum());
        ui->doubleSpinBox_Height->setValue(mono.height()/nsx::mm);
        ui->doubleSpinBox_Width->setValue(mono.width()/nsx::mm);
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
    auto &source = _caller->experiment()->diffractometer()->source();
    auto &mono = source.selectedMonochromator();
    mono.setWavelength(arg1);
}

void SourcePropertyWidget::on_doubleSpinBox_Width_valueChanged(double arg1)
{
    auto &source =_caller->experiment()->diffractometer()->source();
    auto &mono = source.selectedMonochromator();
    mono.setWidth(arg1*nsx::mm);
}

void SourcePropertyWidget::on_doubleSpinBox_Height_valueChanged(double arg1)
{
    auto &source =_caller->experiment()->diffractometer()->source();
    auto &mono = source.selectedMonochromator();
    mono.setHeight(arg1*nsx::mm);
}

void SourcePropertyWidget::on_comboBox_Monochromators_currentIndexChanged(int index)
{
    auto &source =_caller->experiment()->diffractometer()->source();
    source.setSelectedMonochromator(index);

    const auto& mono = source.selectedMonochromator();

    ui->doubleSpinBox_Wavelength->setValue(mono.wavelength());
    ui->doubleSpinBox_FWHM->setValue(mono.fullWidthHalfMaximum());
    ui->doubleSpinBox_Height->setValue(mono.height()/nsx::mm);
    ui->doubleSpinBox_Width->setValue(mono.width()/nsx::mm);
}
