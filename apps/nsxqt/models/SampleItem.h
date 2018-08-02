#pragma once

#include <QJsonObject>
#include <QList>

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class UnitCellItem;

class SampleItem : public InspectableTreeItem
{
public:
    explicit SampleItem();

    QList<UnitCellItem*> unitCellItems();

    void addUnitCell(); 

    QWidget* inspectItem() override;

    void openIsotopesDatabase();
};
