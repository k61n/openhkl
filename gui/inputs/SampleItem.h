#pragma once

#include "InspectableTreeItem.h"
#include <core/PeakList.h>

class UnitCellItem;

class SampleItem : public InspectableTreeItem {
public:
    explicit SampleItem();

    QWidget* inspectItem() override;

    void openSampleGlobalOffsetsFrame();
};
