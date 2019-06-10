//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SampleShapeItem.h
//! @brief     Defines class SampleShapeItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "core/instrument/InstrumentTypes.h"

#include "apps/models/InspectableTreeItem.h"

class SampleShapeItem : public InspectableTreeItem {
public:
    SampleShapeItem();
    ~SampleShapeItem() = default;
    QWidget* inspectItem();
};
