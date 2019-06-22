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

#include "gui/panels/TabInstrument.h"

//-------------------------------------------------------------------------------------------------
//! @class TabInstrument

TabInstrument::TabInstrument() : QcrTabWidget {"instrument"}
{
    // layout
    addTab((detector = new DetectorProperty), "Detector");
    addTab((monoSource = new SourceProperty), "Monochromatic source");
    addTab((sample = new SampleShapeProperties), "Sample");

    setRemake([=]() {
        detector->remake();
        monoSource->remake();
        sample->remake();
    });
}

void TabInstrument::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}
