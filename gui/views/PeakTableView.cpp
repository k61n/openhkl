//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/views/PeakTableView.cpp
//! @brief     Implements classes PeaksTableModel, PeakTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/views/PeakTableView.h"

#include <QHeaderView>

PeakTableView::PeakTableView(QWidget* parent) : QTableView(parent)
{
    // setEditTriggers(QAbstractItemView::SelectedClicked);

    // Selection of a cell in the table select the whole line.
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    verticalHeader()->show();
    setFocusPolicy(Qt::StrongFocus);

    // connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(selectPeak(QModelIndex)));

    // connect(this, SIGNAL(doubleClicked(QModelIndex)), this,
    // SLOT(togglePeakSelection(QModelIndex)));
}

// void PeakTableView::selectPeak(QModelIndex index)
// {
//     if (!index.isValid())
//         return;

//     PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(model());

//     peaks_model->selectPeak(index);
// }

// void PeakTableView::keyPressEvent(QKeyEvent* event)
// {
//     QModelIndex previous_index = currentIndex();

//     QTableView::keyPressEvent(event);

//     QModelIndex current_index = currentIndex();

//     int key = event->key();

//     if (event->modifiers() == Qt::NoModifier) {
//         if (key == Qt::Key_Down || key == Qt::Key_Tab || key == Qt::Key_PageDown) {
//             if (current_index == previous_index)
//                 setCurrentIndex(model()->index(0, 0));
//             selectPeak(currentIndex());
//         } else if (key == Qt::Key_Up || key == Qt::Key_Backspace || key == Qt::Key_PageDown) {
//             if (current_index == previous_index)
//                 setCurrentIndex(model()->index(model()->rowCount() - 1, 0));
//             selectPeak(currentIndex());
//         } else if (key == Qt::Key_Return || key == Qt::Key_Space)
//             togglePeakSelection(currentIndex());
//     }
// }

// void PeakTableView::togglePeakSelection(QModelIndex index)
// {
//     PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(model());
//     if (peaks_model == nullptr)
//         return;

//     peaks_model->togglePeakSelection(index);
// }

// void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
// {
//     PeaksTableModel* peaksModel = dynamic_cast<PeaksTableModel*>(model());
//     if (peaksModel == nullptr)
//         return;

//     const nsx::PeakList& peaks = peaksModel->peaks();
//     if (peaks.empty())
//         return;
//     // Show all peaks as selected when context menu is requested
//     QMenu* menu = new QMenu(this);
//     //
//     QAction* sortbyEquivalence = new QAction("Sort by equivalences", menu);
//     menu->addAction(sortbyEquivalence);
//     connect(sortbyEquivalence, SIGNAL(triggered()), peaksModel, SLOT(sortEquivalents()));

//     QAction* normalize = new QAction("Normalize to monitor", menu);
//     menu->addSeparator();
//     menu->addAction(normalize);

//     // Menu to plot against metadata
//     QModelIndexList indexList = selectionModel()->selectedIndexes();

//     if (indexList.size()) {
//         QMenu* plotasmenu = menu->addMenu("Plot as");
//         const nsx::MetaData& metadata = peaks[indexList[0].row()]->data()->reader()->metadata();
//         const nsx::MetaDataKeySet& keys = metadata.keys();
//         for (const std::string& key : keys) {
//             try {
//                 // Ensure metadata is a Numeric type
//                 metadata.key<double>(key);
//             } catch (std::exception& e) {
//                 continue;
//             }
//             QAction* newparam = new QAction(QString::fromStdString(key), plotasmenu);
//             connect(newparam, &QAction::triggered, this, [&]() { plotAs(key); });
//             plotasmenu->addAction(newparam);
//         }
//     }
//     menu->addSeparator();
//     QMenu* selectionMenu = menu->addMenu("Selection");
//     QAction* selectAllPeaks = new QAction("all peaks", menu);
//     QAction* selectValidPeaks = new QAction("valid peaks", menu);
//     QAction* selectUnindexedPeaks = new QAction("unindexed peaks", menu);
//     QAction* clearSelectedPeaks = new QAction("clear selection", menu);
//     QAction* togglePeaksSelection = new QAction("toggle", menu);
//     selectionMenu->addAction(selectAllPeaks);
//     selectionMenu->addAction(selectValidPeaks);
//     selectionMenu->addAction(selectUnindexedPeaks);
//     selectionMenu->addSeparator();
//     selectionMenu->addAction(clearSelectedPeaks);
//     selectionMenu->addSeparator();
//     selectionMenu->addAction(togglePeaksSelection);

//     connect(normalize, SIGNAL(triggered()), this, SLOT(normalizeToMonitor()));
//     menu->popup(event->globalPos());

//     connect(clearSelectedPeaks, SIGNAL(triggered()), this, SLOT(clearSelectedPeaks()));
//     connect(selectAllPeaks, SIGNAL(triggered()), this, SLOT(selectAllPeaks()));
//     connect(selectValidPeaks, SIGNAL(triggered()), this, SLOT(selectValidPeaks()));
//     connect(selectUnindexedPeaks, SIGNAL(triggered()), this, SLOT(selectUnindexedPeaks()));
//     connect(togglePeaksSelection, SIGNAL(triggered()), this, SLOT(togglePeaksSelection()));
// }

// void PeakTableView::normalizeToMonitor()
// {
//     bool ok;
//     double factor =
//         QInputDialog::getDouble(this, "Enter normalization factor", "", 1, 1, 100000000, 1, &ok);

//     if (!ok)
//         return;

//     PeaksTableModel* peaksModel = dynamic_cast<PeaksTableModel*>(model());
//     if (peaksModel == nullptr)
//         return;

//     nsx::PeakList peaks = peaksModel->peaks();
//     if (peaks.empty())
//         return;

//     peaksModel->normalizeToMonitor(factor);

//     // Keep track of the last selected index before rebuilding the table
//     QModelIndex index = currentIndex();

//     selectRow(index.row());

//     // If no row selected do nothing else.
//     if (!index.isValid())
//         return;
//     nsx::sptrPeak3D peak = peaks[index.row()];
//     emit plotPeak(peak);
// }

// void PeakTableView::plotAs(const std::string& key)
// {
//     QModelIndexList indexList = selectionModel()->selectedIndexes();
//     if (!indexList.size())
//         return;

//     PeaksTableModel* peaksModel = dynamic_cast<PeaksTableModel*>(model());
//     if (peaksModel == nullptr)
//         return;

//     nsx::PeakList peaks = peaksModel->peaks();
//     if (peaks.empty())
//         return;

//     int nPoints = indexList.size();

//     QVector<double> x(nPoints);
//     QVector<double> y(nPoints);
//     QVector<double> e(nPoints);

//     for (int i = 0; i < nPoints; ++i) {
//         nsx::sptrPeak3D p = peaks[indexList[i].row()];
//         x[i] = p->data()->reader()->metadata().key<double>(key);
//         y[i] = p->correctedIntensity().value();
//         e[i] = p->correctedIntensity().sigma();
//     }
//     // gGui->plotData(x, y, e);
// }

// void PeakTableView::selectUnindexedPeaks()
// {
//     PeaksTableModel* peaksModel = dynamic_cast<PeaksTableModel*>(model());
//     if (peaksModel == nullptr)
//         return;
//     QModelIndexList unindexedPeaks = peaksModel->unindexedPeaks();

//     for (QModelIndex index : unindexedPeaks)
//         selectRow(index.row());
// }

// void PeakTableView::togglePeaksSelection()
// {
//     QItemSelectionModel* selection = selectionModel();

//     for (int i = 0; i < model()->rowCount(); ++i) {
//         selection->select(
//             model()->index(i, 0), QItemSelectionModel::Rows | QItemSelectionModel::Toggle);
//     }
// }

// void PeakTableView::selectValidPeaks()
// {
//     PeaksTableModel* peaksModel = dynamic_cast<PeaksTableModel*>(model());
//     if (peaksModel == nullptr)
//         return;
//     QModelIndexList validPeaksIndexes = peaksModel->selectedPeaks();

//     for (QModelIndex index : validPeaksIndexes)
//         selectRow(index.row());
// }
