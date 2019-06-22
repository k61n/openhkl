//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/SourcePropertyWidget.cpp
//! @brief     Implements class SourcePropertyWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QComboBox>
#include <QDoubleSpinBox>

#include "base/logger/Logger.h"
#include "base/utils/Units.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentTypes.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Source.h"

#include "apps/models/SourceItem.h"
#include "apps/tree/SourcePropertyWidget.h"

#include "ui_SourcePropertyWidget.h"

SourcePropertyWidget::SourcePropertyWidget(SourceItem* caller, QWidget* parent)
    : QWidget(parent), _ui(new Ui::SourcePropertyWidget), _caller(caller)
{
    _ui->setupUi(this);

    const auto& source = _caller->experiment()->diffractometer()->source();

    const auto& monos = source.monochromators();

    for (auto&& m : monos)
        _ui->monochromators->addItem(QString::fromStdString(m.name()));

    connect(
        _ui->monochromators, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int index) { onSelectedMonochromatorChanged(index); });
    connect(
        _ui->height, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double height) { onHeightChanged(height); });
    connect(
        _ui->wavelength,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double wavelength) { onWavelengthChanged(wavelength); });
    connect(
        _ui->width, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double width) { onWidthChanged(width); });

    onSelectedMonochromatorChanged(0);
}

SourcePropertyWidget::~SourcePropertyWidget()
{
    delete _ui;
}

void SourcePropertyWidget::onWavelengthChanged(double wavelength)
{
    auto& source = _caller->experiment()->diffractometer()->source();
    auto& mono = source.selectedMonochromator();
    mono.setWavelength(wavelength);
}

void SourcePropertyWidget::onWidthChanged(double width)
{
    auto& source = _caller->experiment()->diffractometer()->source();
    auto& mono = source.selectedMonochromator();
    mono.setWidth(width * nsx::mm);
}

void SourcePropertyWidget::onHeightChanged(double height)
{
    auto& source = _caller->experiment()->diffractometer()->source();
    auto& mono = source.selectedMonochromator();
    mono.setHeight(height * nsx::mm);
}

void SourcePropertyWidget::onSelectedMonochromatorChanged(int index)
{
    auto& source = _caller->experiment()->diffractometer()->source();
    source.setSelectedMonochromator(index);

    const auto& mono = source.selectedMonochromator();

    _ui->wavelength->setValue(mono.wavelength());
    _ui->fwhm->setValue(mono.fullWidthHalfMaximum());
    _ui->height->setValue(mono.height() / nsx::mm);
    _ui->width->setValue(mono.width() / nsx::mm);
}
