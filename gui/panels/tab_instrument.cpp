//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/tab_instrument.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/tab_instrument.h"

//-------------------------------------------------------------------------------------------------
//! @class TabInstrument

TabInstrument::TabInstrument() : QcrTabWidget {"instrument"}
{
    // layout
    addTab((detector = new SubtabDetector), "Detector");
    addTab((monoSource = new SubtabMonochromatic), "Monochromatic source");
    addTab((sample = new SubtabSample), "Sample");
}

void TabInstrument::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}
