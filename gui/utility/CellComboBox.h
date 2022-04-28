//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/CellComboBox.h
//! @brief     Defines class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_CELLCOMBO_H
#define NSX_GUI_UTILITY_CELLCOMBO_H

#include "tables/crystal/UnitCell.h"
#include <QComboBox>

namespace nsx
{
class UnitCell;
}

using CellList = std::vector<nsx::sptrUnitCell>;

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class CellComboBox : public QComboBox {
    Q_OBJECT

 public:
    CellComboBox(QWidget* parent = nullptr);

    //! Add a cell via its pointer
    void addCell(const nsx::sptrUnitCell& cell);

    //! Add a list of unit cells
    void addCells(const std::vector<nsx::sptrUnitCell>& cells);

    //! Clear all elements
    void clearAll();

    //! Return a pointer to the current unit cell
    nsx::sptrUnitCell currentCell() const;

    //! Set the cell list
    void setCellList(const CellList& list);

    //! Refresh the combo box text
    void refresh();


 private:
    //! Index-sorted list of pointers to unit cells
    static CellList _unit_cells;

};

#endif // NSX_GUI_UTILITY_CELLCOMBO_H
