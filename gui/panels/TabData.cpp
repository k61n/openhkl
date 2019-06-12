//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/TabData.cpp
//! @brief     Implements class TabData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/TabData.h"
#include "gui/properties/NumorProperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabData

TabData::TabData() : QcrWidget {"data"}
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget((property = new NumorProperty));
    setLayout(layout);
}
