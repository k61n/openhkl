//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/PeakModel.cpp
//! @brief     Implements classes PeaksTableModel, PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/PeakModel.h"

#include "base/geometry/ReciprocalVector.h"
#include "core/analyse/PeakFilter.h"
#include "core/detector/Detector.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"


PeaksTableModel::PeaksTableModel(
    const QString& name, nsx::sptrExperiment experiment, const nsx::PeakList& peaks)
    : QcrTableModel {name}, _experiment {experiment}, _peaks {peaks}
{
}

PeaksTableModel::PeaksTableModel(const QString& name, nsx::sptrExperiment experiment)
    : QcrTableModel {name}, _experiment {experiment}
{
}

void PeaksTableModel::slotRemoveUnitCell(nsx::UnitCell* unit_cell)
{
    Q_UNUSED(unit_cell)

    QModelIndex topleft_index = index(0, 0);
    QModelIndex bottomright_index = index(rowCount() - 1, columnCount() - 1);

    emit dataChanged(topleft_index, bottomright_index);
}

void PeaksTableModel::slotChangeMaskedPeaks(const nsx::PeakList& peaks)
{
    for (nsx::sptrPeak3D peak : peaks) {
        auto it = std::find(_peaks.begin(), _peaks.end(), peak);
        if (it == _peaks.end())
            continue;

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
    if (it == _peaks.end())
        return;

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
    if (!indexIsValid(index))
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

QVariant PeaksTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

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
    if (!indexIsValid(index))
        return QVariant();

    Eigen::RowVector3i hkl = {0, 0, 0};
    Eigen::RowVector3d hkl_error = {0.0, 0.0, 0.0};

    int row = index.row();
    int column = index.column();
    nsx::UnitCell* cell = _peaks[row]->unitCell();
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

    const Eigen::Vector3d& peak_center = _peaks[row]->shape().center();

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
                    nsx::UnitCell* unit_cell = _peaks[row]->unitCell();
                    if (unit_cell)
                        return QString::fromStdString(unit_cell->name());
                    else
                        return QString("not set");
                }
                case Column::d: {
                    return peak_d;
                }
            }
            break;
        case Qt::ForegroundRole: {
            if (_peaks[row]->enabled())
                return QBrush(Qt::black);
            else
                return QBrush(Qt::red);

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
                nsx::UnitCell* cell1 = p1->unitCell();
                nsx::UnitCell* cell2 = p2->unitCell();
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
                nsx::UnitCell* cell1 = p1->unitCell();
                nsx::UnitCell* cell2 = p2->unitCell();
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
                nsx::UnitCell* cell1 = p1->unitCell();
                nsx::UnitCell* cell2 = p2->unitCell();
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
                const Eigen::Vector3d& center1 = p1->shape().center();
                const Eigen::Vector3d& center2 = p2->shape().center();
                return (center1[0] < center2[0]);
            };
            break;
        }
        case Column::py: {
            compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
                const Eigen::Vector3d& center1 = p1->shape().center();
                const Eigen::Vector3d& center2 = p2->shape().center();
                return (center1[1] < center2[1]);
            };
            break;
        }
        case Column::frame: {
            compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
                const Eigen::Vector3d& center1 = p1->shape().center();
                const Eigen::Vector3d& center2 = p2->shape().center();
                return (center1[2] < center2[2]);
            };
            break;
        }
        case Column::intensity: {
            compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
                double intensity1 = p1->correctedIntensity().value();
                double intensity2 = p2->correctedIntensity().value();
                return (intensity1 < intensity2);
            };
            break;
        }
        case Column::sigmaIntensity: {
            compareFn = [](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
                double sigma_intensity1 = p1->correctedIntensity().sigma();
                double sigma_intensity2 = p2->correctedIntensity().sigma();
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
                nsx::UnitCell* uc1 = p1->unitCell();
                nsx::UnitCell* uc2 = p2->unitCell();
                std::string uc1Name = uc1 ? uc1->name() : "";
                std::string uc2Name = uc2 ? uc2->name() : "";
                return (uc2Name < uc1Name);
            };
            break;
        }
    }
    std::sort(_peaks.begin(), _peaks.end(), compareFn);

    if (order == Qt::DescendingOrder)
        std::reverse(_peaks.begin(), _peaks.end());
    emit layoutChanged();
}

void PeaksTableModel::togglePeakSelection(QModelIndex peak_index)
{
    int row = peak_index.row();

    nsx::sptrPeak3D peak = _peaks[row];

    peak->setSelected(!(peak->selected()));

    QModelIndex topleft_index = index(row, 0);
    QModelIndex bottomright_index = index(row, columnCount() - 1);

    emit dataChanged(topleft_index, bottomright_index);

    // emit _session->signalEnabledPeakChanged(peak);
}

void PeaksTableModel::selectPeak(const QModelIndex& index)
{
    nsx::sptrPeak3D selected_peak = _peaks[index.row()];

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
    nsx::UnitCell* cell = _peaks[0]->unitCell();

    // If no unit cell defined for the peak collection, return.
    if (!cell) {
        //    qWarning() << "No unit cell defined for the peaks";
        return;
    }

    std::sort(_peaks.begin(), _peaks.end(), [&](nsx::sptrPeak3D p1, nsx::sptrPeak3D p2) {
        nsx::MillerIndex miller_index1(p1->q(), *cell);
        nsx::MillerIndex miller_index2(p2->q(), *cell);
        return cell->spaceGroup().isEquivalent(miller_index1, miller_index2);
    });
}

void PeaksTableModel::setUnitCell(nsx::UnitCell* unitCell, QModelIndexList selectedPeaks)
{
    if (selectedPeaks.isEmpty()) {
        for (int i = 0; i < rowCount(); ++i)
            selectedPeaks << index(i, 0);
    }
    for (QModelIndex index : selectedPeaks) {
        nsx::sptrPeak3D peak = _peaks[index.row()];
        peak->setUnitCell(unitCell);
    }
    emit layoutChanged();
    emit unitCellUpdated();
}

void PeaksTableModel::normalizeToMonitor(double factor)
{
    for (nsx::sptrPeak3D peak : _peaks) {
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
        nsx::sptrPeak3D peak = _peaks[i];
        if (!peak->unitCell())
            list.append(index(i, 0));
    }
    return list;
}

QModelIndexList PeaksTableModel::selectedPeaks()
{
    QModelIndexList list;

    for (int i = 0; i < rowCount(); ++i) {
        nsx::sptrPeak3D peak = _peaks[i];
        if (peak->enabled())
            list.append(index(i, 0));
    }
    return list;
}
