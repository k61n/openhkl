//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/tab_data.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/tab_data.h"
#include "nsxgui/gui/properties/numorproperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabData

TabData::TabData() : QcrWidget {"data"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new NumorProperty));
    setLayout(layout);
}
