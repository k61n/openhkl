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

    void filterPeaks(const nsx::PeakList& peaks, const nsx::PeakList& filtered_peaks);

    void openPeakFilterDialog();

    void removeUnitCell(nsx::sptrUnitCell unit_cell);

    void integratePeaks();

    void findSpaceGroup();

    void showPeaksOpenGL();

    void absorptionCorrection();

    void buildShapeLibrary();

    void autoindex();

    void refine();

    void autoAssignUnitCell();
};
