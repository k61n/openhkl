#pragma once

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>

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
