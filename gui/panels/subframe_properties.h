//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_properties.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_PANELS_SUBFRAME_PROPERTIES_H
#define GUI_PANELS_SUBFRAME_PROPERTIES_H

#include "gui/panels/subframe_setup.h"
//@ #include <QCR/widgets/views.h>

class SubframeProperties : public QcrDockWidget {
public:
    SubframeProperties();
    SubframeSetup* tabsframe;
};

#endif // GUI_PANELS_SUBFRAME_PROPERTIES_H
