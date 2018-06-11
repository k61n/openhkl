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
    QList<UnitCellItem*> unitCellItems();
    void addUnitCell(); 
    QWidget* inspectItem() override;
    void openIsotopesDatabase();

private:
    std::vector<nsx::sptrUnitCell> _unitCells;
};
