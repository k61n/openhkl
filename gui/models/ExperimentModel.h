//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/ExperimentModel.h
//! @brief     Defines classes ExperimentTableModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_MODELS_EXPERIMENTMODEL_H
#define OHKL_GUI_MODELS_EXPERIMENTMODEL_H

#include <QAbstractTableModel>

class ExperimentModel : public QAbstractTableModel {
    Q_OBJECT
 public:
    ExperimentModel(QObject* parent = nullptr);
    ~ExperimentModel() = default;

 public:
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    //! Return the data of the item (Manages role interaction)
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

 public:
    //! Returns whether the selected peak is valid
    bool indexIsValid(const QModelIndex& index) const;

 public:
    //! Return the data of the item
    QVariant valueOutput(const QModelIndex& index) const;
    //! Return the data of the item
    QVariant formatOutput(const QModelIndex& index) const;
    //! Manages the display and edition flags
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    //! Returns header information
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

#endif // OHKL_GUI_MODELS_EXPERIMENTMODEL_H