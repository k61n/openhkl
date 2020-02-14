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

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>

//! Property widget of the numor
class NumorProperty : public QWidget {

public:

   //! The constructor
   NumorProperty();
   //! Public call to refresh the widget
   void refreshInput();
   //! Set up the GUI size policies
   void setSizePolicies();

private:

   //! Refresh when changed
   void onChanged();
   //! Clear content
   void clear();
   //! load selector
   void addMenuRequested();

private:

   QTableWidget* _table;
   QComboBox* _numor_selector;

   QPushButton* _add;
   QPushButton* _remove;

   QSizePolicy* _size_policy_widgets;
   QSizePolicy* _size_policy_box;
   QSizePolicy* _size_policy_right;
   QSizePolicy* _size_policy_fixed;
   
};

#endif // GUI_SUBFRAME_EXPERIMENT_PROPERTIES_NUMORPROPERTY_H
