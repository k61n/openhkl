//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SampleItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <core/CrystalTypes.h>
#include <core/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class UnitCellItem;

class SampleItem : public InspectableTreeItem {
public:
    explicit SampleItem();

    QWidget* inspectItem() override;

    void openIsotopesDatabase();

    void openSampleGlobalOffsetsFrame();
};
