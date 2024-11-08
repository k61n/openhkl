//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/PeakCollectionModel.h
//! @brief     Defines classes PeaksTableModel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_MODELS_PEAKCOLLECTIONMODEL_H
#define OHKL_GUI_MODELS_PEAKCOLLECTIONMODEL_H

#include "core/data/DataTypes.h"

#include <QAbstractTableModel>

class PeakCollectionItem;
class PeakItem;

namespace ohkl {
class Peak3D;
}

class PeakCollectionModel : public QAbstractTableModel {
    Q_OBJECT
 public:
    PeakCollectionModel();
    PeakCollectionModel(QObject* parent);
    ~PeakCollectionModel() = default;

    //! Set the root item that will be used within the model
    void setRoot(PeakCollectionItem* peak_collection);
    //! Set the root item that will be used within the model
    PeakCollectionItem* root() const { return _root_item; };
    //! Retrieve the name
    std::string name() const { return _name; };
    //! Retrieve the row count
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    //! retrieve the column count
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    //! Returns whether the selected peak is valid
    bool indexIsValid(const QModelIndex& index) const;
    //! Remove all data
    void reset();
    //! Get the DataSet pointer associated with the PeakCollection
    ohkl::sptrDataSet dataSet() const;
    //! Return the data of the item (Manages role interaction)
    QVariant data(const QModelIndex& index, int role) const override;
    //! Manages the display and edition flags
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    //! Returns header information
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    //! Implement model's setData method for interactable checkbox
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    //! Get the QModelIndex of a PeakItem given its identifier
    QModelIndex getModelIndex(int id) const;
    //! Get the peak item given a peak pointer
    PeakItem* getPeakItem(ohkl::Peak3D* peak) const;

 private:
    PeakCollectionItem* _root_item = nullptr;
    std::string _name;
};

#endif // OHKL_GUI_MODELS_PEAKCOLLECTIONMODEL_H
