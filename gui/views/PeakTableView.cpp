//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/views/PeakTableView.cpp
//! @brief     Implements classes PeaksTableModel, PeakTableView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/views/PeakTableView.h"

#include "gui/items/PeakItem.h"

#include <QAbstractButton>
#include <QHeaderView>

PeakTableView::PeakTableView(QWidget* parent) : QTableView(parent)
{
    // Selection of a cell in the table select the whole line.
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setCornerButtonEnabled(true);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    verticalHeader()->show();
    horizontalHeader()->show();
    setFocusPolicy(Qt::StrongFocus);

    QAbstractButton* cornerButton = findChild<QAbstractButton*>();
    connect(cornerButton, &QAbstractButton::clicked, this, &PeakTableView::resetSort);
}

void PeakTableView::setModel(QAbstractItemModel* model)
{
    _sort_model.setSourceModel(model);
    QTableView::setModel(&_sort_model);
    sortByColumn(PeakColumn::d, Qt::DescendingOrder);
}

void PeakTableView::resetSort()
{
    sortByColumn(-1, Qt::AscendingOrder);
}
