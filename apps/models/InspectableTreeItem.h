//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/InspectableTreeItem.h
//! @brief     Defines class InspectableTreeItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "core/instrument/InstrumentTypes.h"

#include "apps/models/TreeItem.h"

//! Interface for tree Items for which
//! one wants to show internal properties
//! or interact.
class InspectableTreeItem : public TreeItem {
 public:
    InspectableTreeItem();
    ~InspectableTreeItem();
    //! Tree items can expose a QWidget that show their internal properties.
    virtual QWidget* inspectItem() = 0;
};
