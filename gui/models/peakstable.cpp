//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/peakstable.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/models/peakstable.h"

#include "core/crystal/CrystalTypes.h"
#include "core/experiment/DataSet.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/data/IDataReader.h"
#include "core/instrument/InstrumentState.h"
#include "core/logger/Logger.h"
#include "core/data/MetaData.h"
#include "core/crystal/MillerIndex.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakFilter.h"
#include "core/rec_space/ReciprocalVector.h"
#include "core/crystal/UnitCell.h"

#include <QAction>
#include <QHeaderView>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenu>


//!@class PeaksTableModel

PeaksTableModel::PeaksTableModel(
    const QString& name, nsx::sptrExperiment experiment, const nsx::PeakList& peaks,
    QObject* parent)
    : TableModel {name}, _experiment {experiment}, _peaks {peaks}
{
}

PeaksTableModel::PeaksTableModel(
    const QString& name, nsx::sptrExperiment experiment, QObject* parent)
    : TableModel {name}, _experiment {experiment}
{
}

void PeaksTableModel::slotRemoveUnitCell(const nsx::sptrUnitCell unit_cell)
{
    Q_UNUSED(unit_cell)

    QModelIndex topleft_index = index(0, 0);
    QModelIndex bottomright_index = index(rowCount() - 1, columnCount() - 1);

    emit dataChanged(topleft_index, bottomright_index);
}

void PeaksTableModel::slotChangeMaskedPeaks(const nsx::PeakList& peaks)
{
    for (auto peak : peaks) {

        auto it = std::find(_peaks.begin(), _peaks.end(), peak);
        if (it == _peaks.end()) {
            continue;
        }

        int row = std::distance(_peaks.begin(), it);

        QModelIndex topleft_index = index(row, 0);
        QModelIndex bottomright_index = index(row, columnCount() - 1);

        emit dataChanged(topleft_index, bottomright_index);
    }
}

void PeaksTableModel::setPeaks(const nsx::PeakList& peaks)
{
    reset();
    _peaks = peaks;

    QModelIndex topleft_index = index(0, 0);
    QModelIndex bottomright_index = index(rowCount() - 1, columnCount() - 1);

    emit dataChanged(topleft_index, bottomright_index);
}

void PeaksTableModel::slotChangeEnabledPeak(nsx::sptrPeak3D peak)
{
    auto it = std::find(_peaks.begin(), _peaks.end(), peak);
    if (it == _peaks.end()) {
        return;
    }

    int row = std::distance(_peaks.begin(), it);

    QModelIndex topleft_index = index(row, 0);
    QModelIndex bottomright_index = index(row, columnCount() - 1);

    emit dataChanged(topleft_index, bottomright_index);
}

void PeaksTableModel::reset()
{
    beginResetModel();
    _peaks.clear();
    endResetModel();
}

Qt::ItemFlags PeaksTableModel::flags(const QModelIndex& index) const
{
    if (!indexIsValid(index)) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractTableModel::flags(index);
}

QVariant PeaksTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case Column::h: {
            return QString("h");
        }
        case Column::k: {
            return QString("k");
        }
        case Column::l: {
            return QString("l");
        }
        case Column::px: {
            return QString("pixel x");
        }
        case Column::py: {
            return QString("pixel y");
        }
        case Column::frame: {
            return QString("frame");
        }
        case Column::intensity: {
            return QString("intensity");
        }
        case Column::sigmaIntensity: {
            return QString(QChar(0x03C3)) + "(intensity)";
        }
        case Column::numor: {
            return QString("numor");
        }
        case Column::unitCell: {
            return QString("unit cell");
        }
        case Column::d: {
            return QString("d");
        }
        default: return QVariant();
        }
    } else {
        return QVariant(section + 1);
    }
}

QVariant PeaksTableModel::data(const QModelIndex& index, int role) const
{
    if (!indexIsValid(index)) {
        return QVariant();
    }

    Eigen::RowVector3i hkl = {0, 0, 0};
    Eigen::RowVector3d hkl_error = {0.0, 0.0, 0.0};

    int row = index.row();
    int column = index.column();
    auto cell = _peaks[row]->unitCell();
    if (cell) {
        nsx::MillerIndex miller_index(_peaks[row]->q(), *cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            hkl = miller_index.rowVector();
            hkl_error = miller_index.error();
        }
    }
    double peak_d = 1.0 / (_peaks[row]->q().rowVector().norm());

    double intensity = _peaks[row]->correctedIntensity().value();
    double sigma_intensity = _peaks[row]->correctedIntensity().sigma();

    auto peak_center = _peaks[row]->shape().center();

    switch (role) {

    case Qt::DisplayRole:

        switch (column) {
        case Column::h: {
            return hkl(0);
        }
        case Column::k: {
            return hkl(1);
        }
        case Column::l: {
            return hkl(2);
        }
        case Column::px: {
            return peak_center(0);
        }
        case Column::py: {
            return peak_center(1);
        }
        case Column::frame: {
            return peak_center(2);
        }
        case Column::intensity: {
            return intensity;
        }
        case Column::sigmaIntensity: {
            return sigma_intensity;
        }
        case Column::numor: {
            return _peaks[row]->data()->reader()->metadata().key<int>("Numor");
        }
        case Column::unitCell: {
            auto unit_cell = _peaks[row]->unitCell();
            if (unit_cell) {
                return QString::fromStdString(unit_cell->name());
            } else {
                return QString("not set");
            }
        }
        case Column::d: {
            return peak_d;
        }
        }
        break;
    case Qt::ForegroundRole: {

        if (_peaks[row]->enabled()) {
            return QBrush(Qt::black);
        } else {
            return QBrush(Qt::red);
        }

        break;
    }
    case Qt::ToolTipRole:
        switch (column) {
        case Column::h: return hkl[0] + hkl_error[0];
        case Column::k: return hkl[1] + hkl_error[1];
        case Column::l: return hkl[2] + hkl_error[2];
        }
        break;
    }
    return QVariant::Invalid;
}

void PeaksTableModel::sort(int column, Qt::SortOrder order)
{
    std::function<bool(nsx::sptrPeak3D, nsx::sptrPeak3D)> compareFn =
        [](nsx::sptrPeak3D, nsx::sptrPeak3D) { return false; };

    switch (column) {
    case Column::h: {
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            if (cell1 && cell2) {
                nsx::MillerIndex miller_index1(p1->q(), *cell1);
                nsx::MillerIndex miller_index2(p2->q(), *cell2);
                return (miller_index1[0] < miller_index2[0]);
            } else {
                return ((cell1 != nullptr) < (cell2 != nullptr));
            }
        };
        break;
    }
    case Column::k: {
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            if (cell1 && cell2) {
                nsx::MillerIndex miller_index1(p1->q(), *cell1);
                nsx::MillerIndex miller_index2(p2->q(), *cell2);
                return (miller_index1[1] < miller_index2[1]);
            } else {
                return ((cell1 != nullptr) < (cell2 != nullptr));
            }
        };
        break;
    }
    case Column::l: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto cell1 = p1->unitCell();
            auto cell2 = p2->unitCell();
            if (cell1 && cell2) {
                nsx::MillerIndex miller_index1(p1->q(), *cell1);
                nsx::MillerIndex miller_index2(p2->q(), *cell2);
                return (miller_index1[2] < miller_index2[2]);
            } else {
                return ((cell1 != nullptr) < (cell2 != nullptr));
            }
        };
        break;
    }
    case Column::px: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto center1 = p1->shape().center();
            auto center2 = p2->shape().center();
            return (center1[0] < center2[0]);
        };
        break;
    }
    case Column::py: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto center1 = p1->shape().center();
            auto center2 = p2->shape().center();
            return (center1[1] < center2[1]);
        };
        break;
    }
    case Column::frame: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto center1 = p1->shape().center();
            auto center2 = p2->shape().center();
            return (center1[2] < center2[2]);
        };
        break;
    }
    case Column::intensity: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto intensity1 = p1->correctedIntensity().value();
            auto intensity2 = p2->correctedIntensity().value();
            return (intensity1 < intensity2);
        };
        break;
    }
    case Column::sigmaIntensity: {
        compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            auto sigma_intensity1 = p1->correctedIntensity().sigma();
            auto sigma_intensity2 = p2->correctedIntensity().sigma();
            return (sigma_intensity1 < sigma_intensity2);
        };
        break;
    }
    case Column::numor: {
        compareFn = [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
            int numor1 = p1->data()->reader()->metadata().key<int>("Numor");
            int numor2 = p2->data()->reader()->metadata().key<int>("Numor");
            return (numor1 < numor2);
        };
        break;
    }
    case Column::unitCell: {
        compareFn = [&](nsx::sptrPeak3D p1, const nsx::sptrPeak3D p2) {
            auto uc1 = p1->unitCell();
            auto uc2 = p2->unitCell();
            std::string uc1Name = uc1 ? uc1->name() : "";
            std::string uc2Name = uc2 ? uc2->name() : "";
            return (uc2Name < uc1Name);
        };
        break;
    }
    }
    std::sort(_peaks.begin(), _peaks.end(), compareFn);

    if (order == Qt::DescendingOrder) {
        std::reverse(_peaks.begin(), _peaks.end());
    }
    emit layoutChanged();
}

void PeaksTableModel::togglePeakSelection(QModelIndex peak_index)
{
    int row = peak_index.row();

    auto peak = _peaks[row];

    peak->setSelected(!(peak->selected()));

    QModelIndex topleft_index = index(row, 0);
    QModelIndex bottomright_index = index(row, columnCount() - 1);

    emit dataChanged(topleft_index, bottomright_index);

    // emit _session->signalEnabledPeakChanged(peak);
}

void PeaksTableModel::selectPeak(const QModelIndex& index)
{
    auto selected_peak = _peaks[index.row()];

    emit signalSelectedPeakChanged(selected_peak);
}

bool PeaksTableModel::indexIsValid(const QModelIndex& index) const
{
    return index.isValid() && (index.row() < static_cast<int>(_peaks.size()));
}

void PeaksTableModel::sortEquivalents()
{
    // todo: investigate this method. Likely incorrect if there are multiple unit
    // cells.
    auto cell = _peaks[0]->unitCell();

    // If no unit cell defined for the peak collection, return.
    if (!cell) {
        //    nsx::error() << "No unit cell defined for the peaks";
        return;
    }

    std::sort(_peaks.begin(), _peaks.end(), [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
        nsx::MillerIndex miller_index1(p1->q(), *cell);
        nsx::MillerIndex miller_index2(p2->q(), *cell);
        return cell->spaceGroup().isEquivalent(miller_index1, miller_index2);
    });
}

void PeaksTableModel::setUnitCell(const nsx::sptrUnitCell& unitCell, QModelIndexList selectedPeaks)
{
    if (selectedPeaks.isEmpty()) {
        for (int i = 0; i < rowCount(); ++i) {
            selectedPeaks << index(i, 0);
        }
    }
    for (auto&& index : selectedPeaks) {
        auto peak = _peaks[index.row()];
        peak->setUnitCell(unitCell);
    }
    emit layoutChanged();
    emit unitCellUpdated();
}

void PeaksTableModel::normalizeToMonitor(double factor)
{
    for (auto&& peak : _peaks) {
        double monitor = peak->data()->reader()->metadata().key<double>("monitor");
        peak->setScale(factor / monitor);
    }

    QModelIndex topleft_index = index(0, 0);
    QModelIndex bottomright_index = index(rowCount() - 1, columnCount() - 1);

    emit dataChanged(topleft_index, bottomright_index);
}

QModelIndexList PeaksTableModel::unindexedPeaks()
{
    QModelIndexList list;

    for (int i = 0; i < rowCount(); ++i) {
        auto peak = _peaks[i];
        if (!peak->unitCell()) {
            list.append(index(i, 0));
        }
    }
    return list;
}

QModelIndexList PeaksTableModel::selectedPeaks()
{
    QModelIndexList list;

    for (int i = 0; i < rowCount(); ++i) {
        auto peak = _peaks[i];
        if (peak->enabled()) {
            list.append(index(i, 0));
        }
    }
    return list;
}

//  ***********************************************************************************************
// PeaksTableView

PeaksTableView::PeaksTableView(QWidget* parent) : QTableView(parent)
{
    setEditTriggers(QAbstractItemView::SelectedClicked);

    // Selection of a cell in the table select the whole line.
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    verticalHeader()->show();

    setFocusPolicy(Qt::StrongFocus);

    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(selectPeak(QModelIndex)));

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(togglePeakSelection(QModelIndex)));
}

void PeaksTableView::selectPeak(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }

    PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(model());

    peaks_model->selectPeak(index);
}

void PeaksTableView::keyPressEvent(QKeyEvent* event)
{
    auto previous_index = currentIndex();

    QTableView::keyPressEvent(event);

    auto current_index = currentIndex();

    auto key = event->key();

    if (event->modifiers() == Qt::NoModifier) {

        if (key == Qt::Key_Down || key == Qt::Key_Tab || key == Qt::Key_PageDown) {
            if (current_index == previous_index) {
                setCurrentIndex(model()->index(0, 0));
            }
            selectPeak(currentIndex());
        } else if (key == Qt::Key_Up || key == Qt::Key_Backspace || key == Qt::Key_PageDown) {
            if (current_index == previous_index) {
                setCurrentIndex(model()->index(model()->rowCount() - 1, 0));
            }
            selectPeak(currentIndex());
        } else if (key == Qt::Key_Return || key == Qt::Key_Space) {
            togglePeakSelection(currentIndex());
        }
    }
}

void PeaksTableView::togglePeakSelection(QModelIndex index)
{
    auto peaks_model = dynamic_cast<PeaksTableModel*>(model());
    if (peaks_model == nullptr) {
        return;
    }

    peaks_model->togglePeakSelection(index);
}

void PeaksTableView::contextMenuEvent(QContextMenuEvent* event)
{
    auto peaksModel = dynamic_cast<PeaksTableModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }

    auto peaks = peaksModel->peaks();
    if (peaks.empty()) {
        return;
    }
    // Show all peaks as selected when context menu is requested
    auto menu = new QMenu(this);
    //
    QAction* sortbyEquivalence = new QAction("Sort by equivalences", menu);
    menu->addAction(sortbyEquivalence);
    connect(sortbyEquivalence, SIGNAL(triggered()), peaksModel, SLOT(sortEquivalents()));

    auto normalize = new QAction("Normalize to monitor", menu);
    menu->addSeparator();
    menu->addAction(normalize);

    // Menu to plot against metadata
    QModelIndexList indexList = selectionModel()->selectedIndexes();

    if (indexList.size()) {
        QMenu* plotasmenu = menu->addMenu("Plot as");
        const auto& metadata = peaks[indexList[0].row()]->data()->reader()->metadata();
        const auto& keys = metadata.keys();
        for (const auto& key : keys) {
            try {
                // Ensure metadata is a Numeric type
                metadata.key<double>(key);
            } catch (std::exception& e) {
                continue;
            }
            QAction* newparam = new QAction(QString::fromStdString(key), plotasmenu);
            connect(newparam, &QAction::triggered, this, [&]() { plotAs(key); });
            plotasmenu->addAction(newparam);
        }
    }
    menu->addSeparator();
    QMenu* selectionMenu = menu->addMenu("Selection");
    auto selectAllPeaks = new QAction("all peaks", menu);
    auto selectValidPeaks = new QAction("valid peaks", menu);
    auto selectUnindexedPeaks = new QAction("unindexed peaks", menu);
    auto clearSelectedPeaks = new QAction("clear selection", menu);
    auto togglePeaksSelection = new QAction("toggle", menu);
    selectionMenu->addAction(selectAllPeaks);
    selectionMenu->addAction(selectValidPeaks);
    selectionMenu->addAction(selectUnindexedPeaks);
    selectionMenu->addSeparator();
    selectionMenu->addAction(clearSelectedPeaks);
    selectionMenu->addSeparator();
    selectionMenu->addAction(togglePeaksSelection);

    connect(normalize, SIGNAL(triggered()), this, SLOT(normalizeToMonitor()));
    menu->popup(event->globalPos());

    connect(clearSelectedPeaks, SIGNAL(triggered()), this, SLOT(clearSelectedPeaks()));
    connect(selectAllPeaks, SIGNAL(triggered()), this, SLOT(selectAllPeaks()));
    connect(selectValidPeaks, SIGNAL(triggered()), this, SLOT(selectValidPeaks()));
    connect(selectUnindexedPeaks, SIGNAL(triggered()), this, SLOT(selectUnindexedPeaks()));
    connect(togglePeaksSelection, SIGNAL(triggered()), this, SLOT(togglePeaksSelection()));
}

void PeaksTableView::normalizeToMonitor()
{
    bool ok;
    double factor =
        QInputDialog::getDouble(this, "Enter normalization factor", "", 1, 1, 100000000, 1, &ok);

    if (!ok) {
        return;
    }

    auto peaksModel = dynamic_cast<PeaksTableModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }

    auto peaks = peaksModel->peaks();
    if (peaks.empty()) {
        return;
    }

    peaksModel->normalizeToMonitor(factor);

    // Keep track of the last selected index before rebuilding the table
    QModelIndex index = currentIndex();

    selectRow(index.row());

    // If no row selected do nothing else.
    if (!index.isValid()) {
        return;
    }
    nsx::sptrPeak3D peak = peaks[index.row()];
    emit plotPeak(peak);
}

void PeaksTableView::plotAs(const std::string& key)
{
    QModelIndexList indexList = selectionModel()->selectedIndexes();
    if (!indexList.size()) {
        return;
    }

    auto peaksModel = dynamic_cast<PeaksTableModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }

    auto peaks = peaksModel->peaks();
    if (peaks.empty()) {
        return;
    }

    int nPoints = indexList.size();

    QVector<double> x(nPoints);
    QVector<double> y(nPoints);
    QVector<double> e(nPoints);

    for (int i = 0; i < nPoints; ++i) {
        nsx::sptrPeak3D p = peaks[indexList[i].row()];
        x[i] = p->data()->reader()->metadata().key<double>(key);
        y[i] = p->correctedIntensity().value();
        e[i] = p->correctedIntensity().sigma();
    }
    emit plotData(x, y, e);
}

void PeaksTableView::selectUnindexedPeaks()
{
    auto peaksModel = dynamic_cast<PeaksTableModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    QModelIndexList unindexedPeaks = peaksModel->unindexedPeaks();

    for (QModelIndex index : unindexedPeaks) {
        selectRow(index.row());
    }
}

void PeaksTableView::togglePeaksSelection()
{
    QItemSelectionModel* selection = selectionModel();

    for (int i = 0; i < model()->rowCount(); ++i) {
        selection->select(
            model()->index(i, 0), QItemSelectionModel::Rows | QItemSelectionModel::Toggle);
    }
}

void PeaksTableView::selectValidPeaks()
{
    auto peaksModel = dynamic_cast<PeaksTableModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    QModelIndexList validPeaksIndexes = peaksModel->selectedPeaks();

    for (QModelIndex index : validPeaksIndexes) {
        selectRow(index.row());
    }
}
