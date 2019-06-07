//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/tab_data.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************



#ifndef NSXGUI_GUI_PANELS_TAB_DATA_H
#define NSXGUI_GUI_PANELS_TAB_DATA_H

#include "nsxgui/gui/properties/numorproperty.h"
#include <QCR/widgets/views.h>

class TabData : public QcrWidget {
public:
    TabData();

private:
    NumorProperty* property;
};

#endif // NSXGUI_GUI_PANELS_TAB_DATA_H
