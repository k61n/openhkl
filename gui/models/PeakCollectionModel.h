//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/PeakCollectionModel.h
//! @brief     Defines classes PeaksTableModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_MODELS_PEAKCOLLECTIONMODEL_H
#define OHKL_GUI_MODELS_PEAKCOLLECTIONMODEL_H

#include <QAbstractTableModel>

class PeakCollectionItem;

class PeakCollectionModel : public QAbstractTableModel {
    Q_OBJECT
 public:
    PeakCollectionModel();
    PeakCollectionModel(QObject* parent);
    ~PeakCollectionModel() = default;

 public:
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

 public:
    //! Return the data of the item (Manages role interaction)
    QVariant data(const QModelIndex& index, int role) const override;
    //! Manages the display and edition flags
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    //! Returns header information
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    //! Enumerator class for the model
    enum Column {
        h,
        k,
        l,
        px,
        py,
        Frame,
        Intensity,
        Sigma,
        Strength,
        Numor,
        uc,
        d,
        Rejection,
        Filtered,
        Selected,
        Count,
    };
    //! The sorting of the peaks
    void sort(int column, Qt::SortOrder order) override;

 public:
    PeakCollectionItem* _root_item = nullptr;
    std::string _name;
};

#endif // OHKL_GUI_MODELS_PEAKCOLLECTIONMODEL_H
