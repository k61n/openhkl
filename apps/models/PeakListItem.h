//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/PeakListItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "core/peak/Peak3D.h"

#include "apps/models/InspectableTreeItem.h"
class PeakListItem : public InspectableTreeItem {
public:
    explicit PeakListItem(const nsx::PeakList& peaks);

    QWidget* inspectItem();

    nsx::PeakList& peaks();

    void removePeak(nsx::sptrPeak3D peak);

private:
    nsx::PeakList _peaks;
};
