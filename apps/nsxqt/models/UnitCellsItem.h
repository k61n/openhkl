#pragma once

#include "TreeItem.h"

class UnitCellsItem : public TreeItem
{
public:
    explicit UnitCellsItem();

    nsx::UnitCellList unitCells();

    void removeUnusedUnitCells();
};
