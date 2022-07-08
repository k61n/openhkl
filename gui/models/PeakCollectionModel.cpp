//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/PeakCollectionModel.cpp
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/PeakCollectionModel.h"

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
    // if (role != Qt::DisplayRole)
    //     return QVariant();
    if (!indexIsValid(index))
        return QVariant();
    return _root_item->data(index, role);
}

Qt::ItemFlags PeakCollectionModel::flags(const QModelIndex& index) const
{
    if (!indexIsValid(index))
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index);
}

QVariant PeakCollectionModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return QString("x pixel");
            }
            case Column::py: {
                return QString("y pixel");
            }
            case Column::Frame: {
                return QString("Frame");
            }
            case Column::Intensity: {
                return QString("Intensity");
            }
            case Column::Sigma: {
                return QString(QChar(0x03C3)) + "(Int.)";
            }
            case Column::Strength: {
                return QString("Strength");
            }
            case Column::Numor: {
                return QString("Numor");
            }
            case Column::uc: {
                return QString("Unit cell");
            }
            case Column::d: {
                return QString("d");
            }
            case Column::Rejection: {
                return QString("Reason for rejection");
            }
            case Column::Filtered: {
                return QString("Caught by filter");
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
