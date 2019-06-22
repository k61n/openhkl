//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/DetectorItem.h
//! @brief     Defines class DetectorItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "apps/models/InspectableTreeItem.h"

class QWidget;

class DetectorItem : public InspectableTreeItem {
 public:
    explicit DetectorItem();
    QWidget* inspectItem() override;

    void openDetectorGlobalOffsetsFrame();
};
