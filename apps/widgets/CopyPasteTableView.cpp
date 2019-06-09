//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/CopyPasteTableView.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QModelIndex>

#include "apps/widgets/CopyPasteTableView.h"

CopyPasteTableView::CopyPasteTableView(QWidget* parent) : QTableView(parent) {}

void CopyPasteTableView::keyPressEvent(QKeyEvent* event)
{

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    // at least one entire row selected
    if (!selectedRows.isEmpty()) {
        if (event->key() == Qt::Key_Insert)
            model()->insertRows(selectedRows.at(0).row(), selectedRows.size());
        else if (event->key() == Qt::Key_Delete)
            model()->removeRows(selectedRows.at(0).row(), selectedRows.size());

    }

    // at least one cell selected
    if (!selectedIndexes().isEmpty()) {
        if (event->key() == Qt::Key_Delete) {
            foreach (QModelIndex index, selectedIndexes()) {
                model()->setData(index, QString());
            }
        } else if (event->matches(QKeySequence::Copy)) {
            QString text;
            QItemSelectionRange range = selectionModel()->selection().first();
            for (auto i = range.top(); i <= range.bottom(); ++i) {
                QStringList rowContents;
                for (auto j = range.left(); j <= range.right(); ++j)
                    rowContents << model()->index(i, j).data().toString();
                text += rowContents.join("\t");
                text += "\n";
            }
            QApplication::clipboard()->setText(text);
        } else if (event->matches(QKeySequence::Paste)) {
            QString text = QApplication::clipboard()->text();
            QStringList rowContents = text.split("\n", QString::SkipEmptyParts);

            QModelIndex initIndex = selectedIndexes().at(0);
            auto initRow = initIndex.row();
            auto initCol = initIndex.column();

            for (auto i = 0; i < rowContents.size(); ++i) {
                QStringList columnContents = rowContents.at(i).split("\t");
                for (auto j = 0; j < columnContents.size(); ++j) {
                    model()->setData(
                        model()->index(initRow + i, initCol + j), columnContents.at(j));
                }
            }
        } else {
            QTableView::keyPressEvent(event);
        }
    }
}
