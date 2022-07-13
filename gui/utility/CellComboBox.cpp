//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/CellComboBox.h
//! @brief     Defines class CellComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/CellComboBox.h"

#include <QSignalBlocker>

CellList CellComboBox::_unit_cells;
QVector<CellComboBox*> CellComboBox::_all_combos;

CellComboBox::CellComboBox(QWidget* parent) : QComboBox(parent)
{
    _all_combos.push_back(this);
}

void CellComboBox::addCell(const ohkl::sptrUnitCell& cell)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(cell->name()));
    _unit_cells.push_back(cell);
    refresh();
}

void CellComboBox::addCells(const CellList& cells)
{
    for (auto cell : cells)
        addCell(cell);
}

//! Clear all elements
void CellComboBox::clearAll()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    _unit_cells.clear();
}

//! Return a pointer to the current unit cell
ohkl::sptrUnitCell CellComboBox::currentCell() const
{
    if (count() != _unit_cells.size())
        throw std::runtime_error("CellComboBox needs refreshing");
    return _unit_cells.at(currentIndex());
}

void CellComboBox::refresh()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    for (ohkl::sptrUnitCell& cell : _unit_cells)
        addItem(QString::fromStdString(cell->name()));
    setCurrentText(_current);
}

void CellComboBox::refreshAll()
{
    for (auto* combo : _all_combos)
        combo->refresh();
}
