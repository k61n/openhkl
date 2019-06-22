//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SampleItem.h
//! @brief     Defines class SampleItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "core/instrument/InstrumentTypes.h"
#include "core/peak/Peak3D.h"

#include "apps/models/InspectableTreeItem.h"

class UnitCellItem;

class SampleItem : public InspectableTreeItem {
 public:
    explicit SampleItem();

    QWidget* inspectItem() override;

    void openIsotopesDatabase();

    void openSampleGlobalOffsetsFrame();
};
