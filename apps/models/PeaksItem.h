//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/PeaksItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <core/DataTypes.h>
#include <core/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class PeakListItem;

class PeaksItem : public InspectableTreeItem {

public:
    explicit PeaksItem();

    virtual QWidget* inspectItem() override;

    nsx::PeakList allPeaks();

    nsx::PeakList selectedPeaks();

    void openAutoIndexingFrame();

    void openPeakFilterDialog();

    void removeUnitCell(nsx::sptrUnitCell unit_cell);

    void integratePeaks();

    void findSpaceGroup();

    void showPeaksOpenGL();

    void absorptionCorrection();

    void buildShapeLibrary();

    void refine();

    void autoAssignUnitCell();

    void openUserDefinedUnitCellIndexerFrame();

    void normalizeToMonitor();
};
