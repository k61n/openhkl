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

#include "core/data/DataSet.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/items/PeakItem.h"

#include <QModelIndex>

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

ohkl::sptrDataSet PeakCollectionModel::dataSet() const
{
    if (!_root_item)
        return nullptr;
    return _root_item->peakCollection()->data();
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
    if (role == Qt::CheckStateRole && index.column() == PeakColumn::Enabled)
        return _root_item->data(index, role);
    if (!indexIsValid(index))
        return QVariant();
    return _root_item->data(index, role);
}

Qt::ItemFlags PeakCollectionModel::flags(const QModelIndex& index) const
{
    if (!indexIsValid(index))
        return Qt::ItemIsEnabled;
    if (index.column() == PeakColumn::Enabled)
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
            case PeakColumn::SumIntensity: {
                return QString("I (sum)");
            }
            case PeakColumn::SumSigma: {
                return QString(QChar(0x03C3)) + " (sum)";
            }
            case PeakColumn::ProfileIntensity: {
                return QString("I (profile)");
            }
            case PeakColumn::ProfileSigma: {
                return QString(QChar(0x03C3)) + " (profile)";
            }
            case PeakColumn::SumStrength: {
                return QString("I/") + QChar(0x03C3) + QString(" (sum)");
            }
            case PeakColumn::ProfileStrength: {
                return QString("I/") + QChar(0x03C3) + QString(" (profile)");
            }
            case PeakColumn::SumBkg: {
                return QString("B (sum)");
            }
            case PeakColumn::SumBkgSigma: {
                return QString(QChar(0x03C3)) + " (B, sum)";
            }
            case PeakColumn::ProfileBkg: {
                return QString("B (profile)");
            }
            case PeakColumn::ProfileBkgSigma: {
                return QString(QChar(0x03C3)) + " (B, profile)";
            }
            case PeakColumn::BkgGradient: {
                return QString("Gradient");
            }
            case PeakColumn::BkgGradientSigma: {
                return QString(QChar(0x03C3)) + " (Grad.)";
            }
            case PeakColumn::DataSet: {
                return QString("Data set");
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
            case PeakColumn::Enabled: {
                return QString("Valid");
            }
            default: return QVariant();
        }
    } else {
        return QVariant(section + 1);
    }
}

void PeakCollectionModel::reset()
{
    _root_item = nullptr;
    _name = "No Collection";
}

bool PeakCollectionModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;
    if (role == Qt::CheckStateRole && index.column() == PeakColumn::Enabled) {
        if ((Qt::CheckState)value.toInt() == Qt::Checked) {
            _root_item->peakItemAt(index.row())
                ->peak()
                ->setRejectionFlag(ohkl::RejectionFlag::NotRejected, true);
        } else {
            _root_item->peakItemAt(index.row())
                ->peak()
                ->setRejectionFlag(ohkl::RejectionFlag::ManuallyRejected, true);
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QModelIndex PeakCollectionModel::getModelIndex(int id) const
{
    QModelIndex start = index(0, 0);
    QModelIndexList matches = match(start, Role::GetIndex, id, 1);
    return matches.at(0);
}

PeakItem* PeakCollectionModel::getPeakItem(ohkl::Peak3D* peak) const
{
    for (auto* peak_item : _root_item->peakItems()) {
        if (peak_item->peak() == peak)
            return peak_item;
    }
    return nullptr;
}
