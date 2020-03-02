//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/NumorProperty.h
//! @brief     Defines class NumorProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_EXPERIMENT_PROPERTIES_NUMORPROPERTY_H
#define GUI_SUBFRAME_EXPERIMENT_PROPERTIES_NUMORPROPERTY_H

#include <QComboBox>
#include <QTableWidget>
#include <QWidget>

//! Property widget of the numor
class NumorProperty : public QWidget {

 public:
    //! The constructor
    NumorProperty();
    //! Public call to refresh the widget
    void refreshInput();

 private:
    //! Refresh when changed
    void onChanged();
    //! Clear content
    void clear();

 private:
    QTableWidget* _table;
    QComboBox* _numor_selector;
};

#endif // GUI_SUBFRAME_EXPERIMENT_PROPERTIES_NUMORPROPERTY_H
