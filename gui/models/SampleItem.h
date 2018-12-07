#pragma once

#include <core/PeakList.h>
#include <core/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class UnitCellItem;

class SampleItem : public InspectableTreeItem
{
public:
    explicit SampleItem();

    QWidget* inspectItem() override;

    void openIsotopesDatabase();

    void openSampleGlobalOffsetsFrame();
};
