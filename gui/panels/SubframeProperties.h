//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeProperties.h
//! @brief     Defines class SubframeProperties
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAMEPROPERTIES_H
#define GUI_PANELS_SUBFRAMEPROPERTIES_H

#include "gui/panels/SubframeSetup.h"

class SubframeProperties : public QcrDockWidget {
public:
    SubframeProperties();
    SubframeSetup* tabsframe;
};

#endif // GUI_PANELS_SUBFRAMEPROPERTIES_H
