#pragma once

#include "InspectableTreeItem.h"

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>

class QWidget;

class UnitCellItem : public InspectableTreeItem
{
public:
    UnitCellItem(nsx::sptrUnitCell);
    ~UnitCellItem();
    QWidget* inspectItem();
    nsx::sptrUnitCell unitCell();

    void info() const;

    void openTransformationMatrixDialog();
    void openChangeUnitCellDialog();

    void determineSpaceGroup();

private:
    nsx::sptrUnitCell _cell;
};
