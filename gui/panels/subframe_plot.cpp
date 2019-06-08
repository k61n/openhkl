//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_plot.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/subframe_plot.h"
#include "gui/mainwin.h"
#include "gui/view/toggles.h"
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! class SubframePlot

SubframePlot::SubframePlot() : QcrDockWidget {"Plotter"}
{
    setWidget(new QTreeView);
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewPlotter, SLOT(setChecked(bool)));
}
