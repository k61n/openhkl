//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/PeakCollectionModel.cpp
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/PeakCollectionModel.h"

#include "core/peak/Peak3D.h"
#include "gui/items/PeakCollectionItem.h"

PeakCollectionModel::PeakCollectionModel() : QAbstractTableModel()
{
    _root_item = nullptr;
    _name = "No Collection";
}

PeakCollectionModel::PeakCollectionModel(QObject* parent) : QAbstractTableModel(parent)
{
    _root_item = nullptr;
    _name = "No Collection";
}

void PeakCollectionModel::setRoot(PeakCollectionItem* peak_collection)
{
    beginResetModel();
    _root_item = peak_collection;
    _name = _root_item->name();
    endResetModel();
}

int PeakCollectionModel::rowCount(const QModelIndex& /*parent*/) const
{
    if (!_root_item)
        return 0;
    return _root_item->childCount();
}

int PeakCollectionModel::columnCount(const QModelIndex& /*parent*/) const
{
    if (!_root_item)
        return 0;
    return _root_item->columnCount();
}

bool PeakCollectionModel::indexIsValid(const QModelIndex& index) const
{
    if (!_root_item)
        return false;
    return index.isValid() && (index.row() < rowCount());
}

QVariant PeakCollectionModel::data(const QModelIndex& index, int role = Qt::DisplayRole) const
{
    if (role == Qt::CheckStateRole && index.column() == PeakColumn::Selected)
        return _root_item->data(index, role);
    if (!indexIsValid(index))
        return QVariant();
    return _root_item->data(index, role);
}

Qt::ItemFlags PeakCollectionModel::flags(const QModelIndex& index) const
{
    if (!indexIsValid(index))
        return Qt::ItemIsEnabled;
    if (index.column() == PeakColumn::Selected)
        return (QAbstractTableModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    return QAbstractTableModel::flags(index);
}

QVariant PeakCollectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case PeakColumn::h: {
                return QString("h");
            }
            case PeakColumn::k: {
                return QString("k");
            }
            case PeakColumn::l: {
                return QString("l");
            }
            case PeakColumn::px: {
                return QString("x pixel");
            }
            case PeakColumn::py: {
                return QString("y pixel");
            }
            case PeakColumn::Frame: {
                return QString("Frame");
            }
            case PeakColumn::Intensity: {
                return QString("Intensity");
            }
            case PeakColumn::Sigma: {
                return QString(QChar(0x03C3)) + " (Int.)";
            }
            case PeakColumn::Strength: {
                return QString("Strength");
            }
            case PeakColumn::BkgGradient: {
                return QString("Gradient");
            }
            case PeakColumn::BkgGradientSigma: {
                return QString(QChar(0x03C3)) + " (Grad.)";
            }
            case PeakColumn::Numor: {
                return QString("Numor");
            }
            case PeakColumn::uc: {
                return QString("Unit cell");
            }
            case PeakColumn::d: {
                return QString("d");
            }
            case PeakColumn::Rejection: {
                return QString("Reason for rejection");
            }
            case PeakColumn::Filtered: {
                return QString("Caught by filter");
            }
            case PeakColumn::Selected: {
                return QString("Valid");
            }
            default: return QVariant();
        }
    } else {
        return QVariant(section + 1);
    }
}

void PeakCollectionModel::sort(int column, Qt::SortOrder order)
{
    if (!_root_item)
        return;
    return _root_item->sort(column, order);
    emit dataChanged(QModelIndex(), QModelIndex());
}

void PeakCollectionModel::reset()
{
    _root_item = nullptr;
    _name = "No Collection";
}

bool PeakCollectionModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
        return false;
    if (role == Qt::CheckStateRole && index.column() == PeakColumn::Selected) {
        if ((Qt::CheckState)value.toInt() == Qt::Checked) {
            _root_item->peakItemAt(index.row())->peak()->setSelected(true);
        } else {
            _root_item->peakItemAt(index.row())->peak()->setSelected(false);
            _root_item->peakItemAt(index.row())->peak()->setRejectionFlag(
                ohkl::RejectionFlag::ManuallyRejected, true);
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}
