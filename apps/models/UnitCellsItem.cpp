//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/UnitCellsItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <set>

#include <QIcon>

#include <core/CrystalTypes.h>
#include <core/Peak3D.h>

#include "ExperimentItem.h"
#include "MetaTypes.h"
#include "PeaksItem.h"
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

    setCheckable(false);
}

UnitCellItem* UnitCellsItem::selectedUnitCellItem()
{
    for (auto i = 0; i < rowCount(); ++i) {
        auto unit_cell_item = dynamic_cast<UnitCellItem*>(child(i));
        if (!unit_cell_item) {
            continue;
        }

        if (unit_cell_item->checkState() == Qt::Checked) {
            return unit_cell_item;
        }
    }

    return nullptr;
}

nsx::UnitCellList UnitCellsItem::unitCells()
{
    nsx::UnitCellList unitCells;
    unitCells.reserve(rowCount());

    for (int i = 0; i < rowCount(); ++i) {
        UnitCellItem* ucItem = dynamic_cast<UnitCellItem*>(child(i));
        if (ucItem) {
            unitCells.push_back(ucItem->data(Qt::UserRole).value<nsx::sptrUnitCell>());
        }
    }

    return unitCells;
}

void UnitCellsItem::removeUnusedUnitCells()
{
    auto peaks_item = experimentItem()->peaksItem();

    auto&& all_peaks = peaks_item->allPeaks();

    std::set<nsx::sptrUnitCell> unit_cells;

    for (auto peak : all_peaks) {
        auto unit_cell = peak->unitCell();
        if (!unit_cell) {
            continue;
        }
        auto it = unit_cells.find(unit_cell);
        if (it == unit_cells.end()) {
            unit_cells.insert(unit_cell);
        }
    }

    for (int i = rowCount() - 1; i >= 0; --i) {
        UnitCellItem* unit_cell_item = dynamic_cast<UnitCellItem*>(child(i));
        if (unit_cell_item) {
            auto it =
                unit_cells.find(unit_cell_item->data(Qt::UserRole).value<nsx::sptrUnitCell>());
            if (it == unit_cells.end()) {
                removeRow(i);
            }
        }
    }
}
