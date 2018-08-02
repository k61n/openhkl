#pragma once

#include "InspectableTreeItem.h"

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>

class QWidget;

class UnitCellItem : public InspectableTreeItem
{
public:
    UnitCellItem(nsx::sptrUnitCell unit_cell);
    ~UnitCellItem();   

    QVariant data(int role) const;
    void setData(const QVariant& value, int role);

    QWidget* inspectItem();

    nsx::sptrUnitCell unitCell();

    void info() const;

public slots:
    void openIndexingToleranceDialog();
    void openTransformationMatrixDialog();
    void openChangeUnitCellDialog();

    void openSpaceGroupDialog();

private:

    nsx::sptrUnitCell _unit_cell;
};
