#pragma once

#include <QJsonObject>
#include <QList>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class UnitCellItem;

class SampleItem : public InspectableTreeItem
{
public:
    explicit SampleItem();

    QWidget* inspectItem() override;

    nsx::UnitCellList unitCells();

    void openIsotopesDatabase();
};
