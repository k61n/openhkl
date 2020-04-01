//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/TabInstrument.cpp
//! @brief     Implements class TabInstrument
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/InstrumentProperty.h"

InstrumentProperty::InstrumentProperty() : QTabWidget()
{
    addTab((_detector = new DetectorProperty), "Detector");
    addTab((_monoSource = new SourceProperty), "Monochromatic source");
    addTab((_sample = new SampleShapeProperties), "Sample");
}

void InstrumentProperty::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}

void InstrumentProperty::refreshInput() const
{
    _detector->refreshInput();
    _monoSource->refreshInput();
    _sample->refreshInput();
}
