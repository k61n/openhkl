//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SourceItem.h
//! @brief     Defines class SourceItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "apps/models/InspectableTreeItem.h"

class SourceItem : public InspectableTreeItem {
public:
    explicit SourceItem(const char* name);
    QWidget* inspectItem();
};
