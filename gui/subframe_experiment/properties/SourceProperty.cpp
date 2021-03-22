//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/properties/SourceProperty.cpp
//! @brief     Implements class SourceProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/SourceProperty.h"
#include "base/utils/Units.h"
#include "core/experiment/Experiment.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QFormLayout>

SourceProperty::SourceProperty()
{
    QFormLayout* form = new QFormLayout(this);

    _type = new QComboBox();
    _type->setEnabled(false);

    _monochromators = new QComboBox();
    _monochromators->setEnabled(true);

    _wavelength = new QDoubleSpinBox();
    _wavelength->setButtonSymbols(QDoubleSpinBox::NoButtons);

    _FWHM = new QDoubleSpinBox();
    _FWHM->setButtonSymbols(QDoubleSpinBox::NoButtons);

    _width = new QDoubleSpinBox();
    _width->setButtonSymbols(QDoubleSpinBox::NoButtons);

    _height = new QDoubleSpinBox();
    _height->setButtonSymbols(QDoubleSpinBox::NoButtons);

    form->addRow("Type:", _type);
    form->addRow("Monochromators:", _monochromators);
    form->addRow("Wavelength (ang):", _wavelength);
    form->addRow("FWHM (ang):", _FWHM);
    form->addRow("Width (mm):", _width);
    form->addRow("Height (mm):", _height);
    form->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QStringList list;
    int exp = gSession->currentProjectNum();
    if (exp < 0)
        _monochromators->addItems(list);
    else {
        const std::vector<nsx::Monochromator>& monos = gSession->currentProject()
                                                           ->experiment()
                                                           ->getDiffractometer()
                                                           ->source()
                                                           .monochromators();
        for (nsx::Monochromator m : monos)
            list.push_back(QString::fromStdString(m.name()));
        _monochromators->addItems(list);
    }
}

void SourceProperty::refreshInput()
{
    if (gSession->currentProjectNum() >= 0)
        onMonoChanged(0);
    else
        clear();
}

void SourceProperty::clear()
{
    _wavelength->setValue(0.00);
    _FWHM->setValue(0.00);
    _width->setValue(0.00);
    _height->setValue(0.00);
}

void SourceProperty::onMonoChanged(int index)
{
    nsx::Source& source = gSession->currentProject()->experiment()->getDiffractometer()->source();
    source.setSelectedMonochromator(index);

    const nsx::Monochromator& mono = source.selectedMonochromator();

    _wavelength->setValue(mono.wavelength());
    _FWHM->setValue(mono.fullWidthHalfMaximum());
    _height->setValue(mono.height() / nsx::mm);
    _width->setValue(mono.width() / nsx::mm);
}

void SourceProperty::onWavelength(double wavelength)
{
    nsx::Source& source = gSession->currentProject()->experiment()->getDiffractometer()->source();
    nsx::Monochromator& mono = source.selectedMonochromator();
    mono.setWavelength(wavelength);
}

void SourceProperty::onWidth(double width)
{
    nsx::Source& source = gSession->currentProject()->experiment()->getDiffractometer()->source();
    nsx::Monochromator& mono = source.selectedMonochromator();
    mono.setWidth(width * nsx::mm);
}

void SourceProperty::onHeight(double height)
{
    nsx::Source& source = gSession->currentProject()->experiment()->getDiffractometer()->source();
    nsx::Monochromator& mono = source.selectedMonochromator();
    mono.setHeight(height * nsx::mm);
}
