//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_properties.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/subframe_properties.h"
#include "gui/mainwin.h"
#include "gui/panels/subframe_setup.h"
#include "gui/view/toggles.h"

SubframeProperties::SubframeProperties() : QcrDockWidget {"Properties"}
{
    setWidget((tabsframe = new SubframeSetup));
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewProperties,
        SLOT(setChecked(bool)));
}
