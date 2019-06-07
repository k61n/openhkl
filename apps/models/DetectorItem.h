//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/DetectorItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "InspectableTreeItem.h"

class QWidget;

class DetectorItem : public InspectableTreeItem {
public:
    explicit DetectorItem();
    QWidget* inspectItem() override;

    void openDetectorGlobalOffsetsFrame();
};
