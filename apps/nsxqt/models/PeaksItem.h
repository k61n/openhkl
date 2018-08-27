#pragma once

#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class PeakListItem;

class PeaksItem: public InspectableTreeItem {

public:

    explicit PeaksItem();

    virtual QWidget* inspectItem() override;

    nsx::PeakList allPeaks();

    nsx::PeakList selectedPeaks();

    void removeUnitCell(nsx::sptrUnitCell unit_cell);

    void integratePeaks();
    void findSpaceGroup();
    void showPeaksOpenGL();
    void absorptionCorrection();
    void buildShapeLibrary();
    void filterPeaks();
    void autoindex();
    void refine();
    void autoAssignUnitCell();
};
