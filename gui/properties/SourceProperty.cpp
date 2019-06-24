//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/SourceProperty.cpp
//! @brief     Implements class SourceProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/properties/SourceProperty.h"

#include "base/utils/Units.h"
#include "gui/models/Session.h"
#include <QCR/engine/cell.h>
#include <QFormLayout>

SourceProperty::SourceProperty() : QcrWidget("sourceProperty")
{
    QFormLayout* form = new QFormLayout(this);

    type = new QcrComboBox("sourcetype", new QcrCell<int>(0), {"Neutron"});
    type->setEnabled(false);

    monochromators = new QcrComboBox("monochromators", new QcrCell<int>(0), []() {
        QStringList list;
        int exp = gSession->selectedExperimentNum();
        if (exp < 0)
            return list;
        const std::vector<nsx::Monochromator>& monos = gSession->selectedExperiment()
                                                           ->experiment()
                                                           ->diffractometer()
                                                           ->source()
                                                           .monochromators();
        for (nsx::Monochromator m : monos)
            list.append(QString::fromStdString(m.name()));
        return list;
    });
    monochromators->setEnabled(true);

    wavelength = new QcrDoubleSpinBox("wavelength", new QcrCell<double>(0.00), 5, 2);
    wavelength->setButtonSymbols(QDoubleSpinBox::NoButtons);
    fwhm = new QcrDoubleSpinBox("fwhm", new QcrCell<double>(0.00), 5, 2);
    fwhm->setButtonSymbols(QDoubleSpinBox::NoButtons);
    width = new QcrDoubleSpinBox("width", new QcrCell<double>(0.00), 5, 2);
    width->setButtonSymbols(QDoubleSpinBox::NoButtons);
    height = new QcrDoubleSpinBox("height", new QcrCell<double>(0.00), 5, 2);
    height->setButtonSymbols(QDoubleSpinBox::NoButtons);
    form->addRow("Type:", type);
    form->addRow("Monochromators:", monochromators);
    form->addRow("Wavelength (ang):", wavelength);
    form->addRow("FWHM (ang):", fwhm);
    form->addRow("Width (mm):", width);
    form->addRow("Height (mm):", height);
    form->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    setRemake([this]() { onRemake(); });
    remake();
}

void SourceProperty::onRemake()
{
    monochromators->remake();
    if (gSession->selectedExperimentNum() >= 0) {
        wavelength->setHook([this](double v) { onWavelength(v); });
        width->setHook([this](double v) { onWidth(v); });
        height->setHook([this](double v) { onHeight(v); });
        onMonoChanged(0);
    } else {
        clear();
    }
}

void SourceProperty::clear()
{
    wavelength->setCellValue(0.00);
    wavelength->setHook([](double) {});
    fwhm->setCellValue(0.00);
    width->setCellValue(0.00);
    width->setHook([](double) {});
    height->setCellValue(0.00);
    height->setHook([](double) {});
}

void SourceProperty::onMonoChanged(int index)
{
    nsx::Source& source = gSession->selectedExperiment()->experiment()->diffractometer()->source();
    source.setSelectedMonochromator(index);

    const nsx::Monochromator& mono = source.selectedMonochromator();

    wavelength->setCellValue(mono.wavelength());
    fwhm->setCellValue(mono.fullWidthHalfMaximum());
    height->setCellValue(mono.height() / nsx::mm);
    width->setCellValue(mono.width() / nsx::mm);
}

void SourceProperty::onWavelength(double wavelength)
{
    nsx::Source& source = gSession->selectedExperiment()->experiment()->diffractometer()->source();
    nsx::Monochromator& mono = source.selectedMonochromator();
    mono.setWavelength(wavelength);
}

void SourceProperty::onWidth(double width)
{
    nsx::Source& source = gSession->selectedExperiment()->experiment()->diffractometer()->source();
    nsx::Monochromator& mono = source.selectedMonochromator();
    mono.setWidth(width * nsx::mm);
}

void SourceProperty::onHeight(double height)
{
    nsx::Source& source = gSession->selectedExperiment()->experiment()->diffractometer()->source();
    nsx::Monochromator& mono = source.selectedMonochromator();
    mono.setHeight(height * nsx::mm);
}
