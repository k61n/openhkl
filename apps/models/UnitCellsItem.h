#pragma once

#include "TreeItem.h"

class UnitCellItem;

class UnitCellsItem : public TreeItem {
public:
    explicit UnitCellsItem();

    nsx::UnitCellList unitCells();

    void removeUnusedUnitCells();

    UnitCellItem* selectedUnitCellItem();
};
