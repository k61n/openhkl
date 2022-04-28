//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/LinkedComboBox.h
//! @brief     Defines class LinkedComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/CellComboBox.h"

#include "tables/crystal/UnitCell.h"

#include <QSignalBlocker>

CellList CellComboBox::_unit_cells;

CellComboBox::CellComboBox(QWidget* parent) : QComboBox(parent)
{
}

void CellComboBox::addCell(const nsx::sptrUnitCell& cell)
{
    addItem(QString::fromStdString(cell->name()));
    _unit_cells.push_back(cell);
    refresh();
}

void CellComboBox::addCells(const std::vector<nsx::sptrUnitCell>& cells)
{
    for (auto cell : cells)
        addCell(cell);
}

//! Clear all elements
void CellComboBox::clearAll()
{
    clear();
    _unit_cells.clear();
}

//! Return a pointer to the current unit cell
nsx::sptrUnitCell CellComboBox::currentCell() const
{
    if (count() != _unit_cells.size())
        throw std::runtime_error("CellComboBox needs refreshing");
    return _unit_cells.at(currentIndex());
}

void CellComboBox::setCellList(const CellList& list)
{
    clearAll();
    _unit_cells = list;
    refresh();
}

void CellComboBox::refresh()
{
    clear();
    for (nsx::sptrUnitCell& cell : _unit_cells)
        addItem(QString::fromStdString(cell->name()));
}
