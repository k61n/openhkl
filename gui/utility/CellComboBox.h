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

#ifndef OHKL_GUI_UTILITY_CELLCOMBO_H
#define OHKL_GUI_UTILITY_CELLCOMBO_H

#include "tables/crystal/UnitCell.h"
#include <QComboBox>

namespace ohkl {
class UnitCell;
}

using CellList = std::vector<ohkl::sptrUnitCell>;

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class CellComboBox : public QComboBox {
    Q_OBJECT

 public:
    CellComboBox(QWidget* parent = nullptr);

    //! Add a cell via its pointer
    void addCell(const ohkl::sptrUnitCell& cell);

    //! Add a list of unit cells
    void addCells(const CellList& cells);

    //! Clear all elements
    void clearAll();

    //! Return a pointer to the current unit cell
    ohkl::sptrUnitCell currentCell() const;

    //! Refresh the combo box text
    void refresh();

    //! Refresh all combos of this type
    void refreshAll();

 private:
    //! Index-sorted list of pointers to unit cells
    static CellList _unit_cells;
    //! Current unit cell
    QString _current;
    //! Vector of all instances to refresh all in one call
    static QVector<CellComboBox*> _all_combos;
};

#endif // OHKL_GUI_UTILITY_CELLCOMBO_H
