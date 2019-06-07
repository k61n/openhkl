//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subframe_properties.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_SUBFRAME_PROPERTIES_H
#define NSXGUI_GUI_PANELS_SUBFRAME_PROPERTIES_H

#pragma once

#include "nsxgui/gui/panels/subframe_setup.h"
#include <QCR/widgets/views.h>

class SubframeProperties : public QcrDockWidget {
public:
    SubframeProperties();
    SubframeSetup* tabsframe;
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_PROPERTIES_H
