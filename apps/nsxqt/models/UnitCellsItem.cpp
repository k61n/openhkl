#include <QIcon>

#include "UnitCellItem.h"
#include "UnitCellsItem.h"

UnitCellsItem::UnitCellsItem() : TreeItem()
{
    setText("Unit cells");

    QIcon icon(":/resources/unitCellsIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);
}

nsx::UnitCellList UnitCellsItem::unitCells()
{
    nsx::UnitCellList unitCells;
    unitCells.reserve(rowCount());

    for (int i = 0; i < rowCount(); ++i)
    {
        UnitCellItem* ucItem = dynamic_cast<UnitCellItem*>(child(i));
        if (ucItem) {
            unitCells.push_back(ucItem->unitCell());
        }
    }

    return unitCells;
}
