//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/views/UnitCellTableView.cpp
//! @brief     Implements classes PeaksTableModel, PeaksTableView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/views/UnitCellTableView.h"

#include <QHeaderView>
#include <QStandardItem>
#include <QString>

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

ValueTupleItem::ValueTupleItem(const QString& text, const double val, const double qmes)
    : QStandardItem(text), value{val}, qmeasure{qmes}
{
}

bool ValueTupleItem::operator<(const QStandardItem& other) const
{
    // assumes that the other Item is also of type ValueTupleItem
    const ValueTupleItem* other_p = dynamic_cast<const ValueTupleItem*>(&other);
    const bool isLess = (this->value < other_p->value)
        || (this->value == other_p->value && this->qmeasure < other_p->qmeasure);

    return isLess;
}
