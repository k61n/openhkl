//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/TabData.h
//! @brief     Defines class TabData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_TABDATA_H
#define GUI_PANELS_TABDATA_H

#include "gui/properties/NumorProperty.h"

class TabData : public QcrWidget {
public:
    TabData();

private:
    NumorProperty* property;
};

#endif // GUI_PANELS_TABDATA_H
