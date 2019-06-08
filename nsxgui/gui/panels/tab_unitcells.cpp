//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/tab_unitcells.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/tab_unitcells.h"
#include "nsxgui/gui/properties/unitcellproperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabUnitcells

TabUnitcells::TabUnitcells() : QcrWidget {"unit cells"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new UnitCellProperty));
    setLayout(layout);
}
