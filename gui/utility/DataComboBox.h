//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/DataComboBox.h
//! @brief     Defines class DataComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_DATACOMBO_H
#define NSX_GUI_UTILITY_DATACOMBO_H

#include "core/data/DataSet.h"

#include <QComboBox>

namespace nsx
{
class DataSet;
}

using DataList = std::vector<nsx::sptrDataSet>;

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class DataComboBox : public QComboBox {
    Q_OBJECT

 public:
    DataComboBox(QWidget* parent = nullptr);

    //! Add a cell via its pointer
    void addDataSet(const nsx::sptrDataSet& data);

    //! Add a list of unit cells
    void addDataSets(const DataList& data_list);

    //! Clear all elements
    void clearAll();

    //! Return a pointer to the current unit cell
    nsx::sptrDataSet currentData() const;

    //! Refresh the combo box text
    void refresh();


 private:
    //! Index-sorted list of pointers to unit cells
    static DataList _data_sets;
    //! Current data set
    QString _current;

};

#endif // NSX_GUI_UTILITY_DATACOMBO_H
