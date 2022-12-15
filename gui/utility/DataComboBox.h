//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/DataComboBox.h
//! @brief     Defines class DataComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_DATACOMBO_H
#define OHKL_GUI_UTILITY_DATACOMBO_H

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"

#include <QComboBox>

namespace ohkl {
class DataSet;
}

using DataList = std::vector<ohkl::sptrDataSet>;

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class DataComboBox : public QComboBox {
    Q_OBJECT

 public:
    DataComboBox(QWidget* parent = nullptr);

    //! Add a cell via its pointer
    void addDataSet(const ohkl::sptrDataSet& data);

    //! Add a list of unit cells
    void addDataSets(const DataList& data_list);

    //! Clear all elements
    void clearAll();

    //! Return a pointer to the current unit cell
    ohkl::sptrDataSet currentData();

    //! Refresh the combo box text
    void refresh();

    //! Refresh all combos of this type
    void refreshAll();

 public slots:
    void onDataChanged(int index);

 private:
    //! Index-sorted list of pointers to unit cells
    static DataList _data_sets;
    //! Current data set
    QString _current_text;
    //! Pointer to current DataSet
    int _current_index;
    //! Vector of all instances to refresh all in one call
    static QVector<DataComboBox*> _all_combos;
};

#endif // OHKL_GUI_UTILITY_DATACOMBO_H
