//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/views/UnitCellTableView.cpp
//! @brief     Implements classes PeaksTableModel, PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/views/UnitCellTableView.h"

#include <QHeaderView>

UnitCellTableView::UnitCellTableView(QWidget* parent) : QTableView(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    verticalHeader()->show();
    setFocusPolicy(Qt::StrongFocus);
}
