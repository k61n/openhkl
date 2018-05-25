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

    void setData(const QVariant& value, int role = Qt::UserRole + 1) override;

    QWidget* inspectItem() override;

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;

    QList<UnitCellItem*> unitCellItems();

    void addUnitCell();

    void openIsotopesDatabase();
};
