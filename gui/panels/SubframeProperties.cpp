//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeProperties.cpp
//! @brief     Implements class SubframeProperties
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/SubframeProperties.h"

#include "gui/actions/Triggers.h"
#include "gui/MainWin.h"
#include "gui/panels/SubframeSetup.h"

SubframeProperties::SubframeProperties() : QcrDockWidget {"Properties"}
{
    setWidget((tabsframe = new SubframeSetup));
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->triggers->viewProperties,
        SLOT(setChecked(bool)));
}
