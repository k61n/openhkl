//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/views/ExperimentTableView.cpp
//! @brief     Implements classes PeaksTableModel, PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/views/ExperimentTableView.h"

#include <QHeaderView>

ExperimentTableView::ExperimentTableView(QWidget* parent) : QTableView(parent)
{
    // Selection of a cell in the table select the whole line.
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    // setSortingEnabled(true);
    verticalHeader()->setVisible(false);
    // sortByColumn(0, Qt::AscendingOrder);
    setFocusPolicy(Qt::StrongFocus);
}
