//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/tab_data.h
//! @brief     Defines class TabData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_TAB_DATA_H
#define GUI_PANELS_TAB_DATA_H

#include "gui/properties/numorproperty.h"

class TabData : public QcrWidget {
public:
    TabData();

private:
    NumorProperty* property;
};

#endif // GUI_PANELS_TAB_DATA_H
