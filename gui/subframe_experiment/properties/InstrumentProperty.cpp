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
    addTab((detector = new DetectorProperty), "Detector");
    addTab((monoSource = new SourceProperty), "Monochromatic source");
    addTab((sample = new SampleShapeProperties), "Sample");
}

void InstrumentProperty::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}
