//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/PeakCollectionModel.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_PEAKCOLLECTIONMODEL_H
#define GUI_MODELS_PEAKCOLLECTIONMODEL_H

#include "gui/items/PeakCollectionItem.h"
#include <QAbstractItemModel>

class PeakCollectionModel : public QAbstractItemModel {
    Q_OBJECT
public:
    PeakCollectionModel();
    PeakCollectionModel(QObject *parent);
    ~PeakCollectionModel() = default;

public:
    //! Set the root item that will be used within the model
    void setRoot(PeakCollectionItem* peak_collection);
    //! Retrieve the row count
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    //!retrieve the column count
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

public:
    //! Return the data of the item (Manages role interaction)
    QVariant data(const QModelIndex& index, int role) const;
    //! Manages the display and edition flags
    Qt::ItemFlags flags(const QModelIndex& index) const;
    //! Returns header information
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    //! Return the index of the item 
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    //!return the parent of a model
    QModelIndex parent(const QModelIndex &index) const;

private:
    PeakCollectionItem* _root_item;

};

#endif // GUI_MODELS_PEAKCOLLECTIONMODEL_H