#include <QComboBox>
#include <QDoubleSpinBox>

#include <core/Diffractometer.h>
#include <core/Logger.h>
#include <core/Monochromator.h>
#include <core/Units.h>

#include "SourcePropertyWidget.h"

#include "ui_SourcePropertyWidget.h"

SourcePropertyWidget::SourcePropertyWidget(nsx::Source& source)
    : QWidget(),
      _ui(new Ui::SourcePropertyWidget),
      _source(source)
{
    _ui->setupUi(this);

    const auto &monos = _source.monochromators();

    for (auto&& m : monos) {
        _ui->monochromators->addItem(QString::fromStdString(m.name()));
    }

    connect(_ui->monochromators,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){onSelectedMonochromatorChanged(index);});
    connect(_ui->height,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double height){onHeightChanged(height);});
    connect(_ui->wavelength,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double wavelength){onWavelengthChanged(wavelength);});
    connect(_ui->width,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double width){onWidthChanged(width);});

    onSelectedMonochromatorChanged(0);
}

SourcePropertyWidget::~SourcePropertyWidget()
{
    delete _ui;
}

void SourcePropertyWidget::onWavelengthChanged(double wavelength)
{
    auto &mono = _source.selectedMonochromator();
    mono.setWavelength(wavelength);
}

void SourcePropertyWidget::onWidthChanged(double width)
{
    auto &mono = _source.selectedMonochromator();
    mono.setWidth(width*nsx::mm);
}

void SourcePropertyWidget::onHeightChanged(double height)
{
    auto &mono = _source.selectedMonochromator();
    mono.setHeight(height*nsx::mm);
}

void SourcePropertyWidget::onSelectedMonochromatorChanged(int index)
{
    _source.setSelectedMonochromator(index);

    const auto &mono = _source.selectedMonochromator();

    _ui->wavelength->setValue(mono.wavelength());
    _ui->fwhm->setValue(mono.fullWidthHalfMaximum());
    _ui->height->setValue(mono.height()/nsx::mm);
    _ui->width->setValue(mono.width()/nsx::mm);
}
