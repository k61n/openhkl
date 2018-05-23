#pragma once

#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>

#include "TreeItem.h"

class PeakListItem;

class PeaksItem: public TreeItem {
public:
    explicit PeaksItem();
    PeakListItem* createPeaksItem(const char* name);
    nsx::PeakList selectedPeaks();
    void integratePeaks();
    void findSpaceGroup();
    void showPeaksOpenGL();
    void absorptionCorrection();
    void buildShapeLibrary();
    void filterPeaks();
    void autoindex();
    void refine();
};
