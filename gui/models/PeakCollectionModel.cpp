//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

PeakCollectionModel::PeakCollectionModel()
    :QAbstractItemModel()
{

}

PeakCollectionModel::PeakCollectionModel(QObject *parent)
    :QAbstractItemModel(parent)
{

}

void PeakCollectionModel::setRoot(PeakCollectionItem* peak_collection)
{
   _root_item = peak_collection;

}

int PeakCollectionModel::rowCount(const QModelIndex & /*parent*/) const
{
   _root_item->childCount();
}

int PeakCollectionModel::columnCount(const QModelIndex & /*parent*/) const
{
    _root_item->columnCount();
}

QVariant PeakCollectionModel::data(
    const QModelIndex &index, 
    int role = Qt::DisplayRole) const 
{
    return _root_item->data(index, role);
}

Qt::ItemFlags PeakCollectionModel::flags(const QModelIndex& index) const
{
    if (checkIndex(index))
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index);
}

QVariant PeakCollectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    // if (orientation == Qt::Horizontal) {
    //     switch (section) {
    //         case Column::h: {
    //             return QString("h");
    //         }
    //         case Column::k: {
    //             return QString("k");
    //         }
    //         case Column::l: {
    //             return QString("l");
    //         }
    //         case Column::px: {
    //             return QString("pixel x");
    //         }
    //         case Column::py: {
    //             return QString("pixel y");
    //         }
    //         case Column::frame: {
    //             return QString("frame");
    //         }
    //         case Column::intensity: {
    //             return QString("intensity");
    //         }
    //         case Column::sigmaIntensity: {
    //             return QString(QChar(0x03C3)) + "(intensity)";
    //         }
    //         case Column::numor: {
    //             return QString("numor");
    //         }
    //         case Column::unitCell: {
    //             return QString("unit cell");
    //         }
    //         case Column::d: {
    //             return QString("d");
    //         }
    //         default: return QVariant();
    //     }
    // } else {
    //     return QVariant(section + 1);
    // }
}

QModelIndex PeakCollectionModel::index(int row, int col, const QModelIndex &parent ) const
{
    if (row >= rowCount())
        return QModelIndex();
    if (col >= columnCount())
        return QModelIndex();
    if ( !(parent.isValid()) )
        return QModelIndex();

    QStandardItem *parentItem = static_cast<QStandardItem*>(parent.internalPointer());
    
    if (!parentItem->hasChildren())
        return QModelIndex();

    return createIndex(row, col, _root_item->child(row));

}

QModelIndex PeakCollectionModel::parent(const QModelIndex &item) const
{
    if ( !(item.isValid()) )
        return QModelIndex();
 
    QStandardItem *childItem = static_cast<QStandardItem*>(item.internalPointer());
    QStandardItem *parentItem = childItem->parent();
 
    if ( parentItem == _root_item )
        return QModelIndex();
 
    return createIndex( parentItem->row(), 0, parentItem );
}
