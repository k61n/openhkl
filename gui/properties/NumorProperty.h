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

#ifndef GUI_PROPERTIES_NUMORPROPERTY_H
#define GUI_PROPERTIES_NUMORPROPERTY_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QTableWidget>

//! Property widget of the numor
class NumorProperty : public QcrWidget {
 public:
    NumorProperty();

 private:
    void onChanged();
    void onRemake();
    void clear();
    QTableWidget* table;
    QcrComboBox* numor;
};

#endif // GUI_PROPERTIES_NUMORPROPERTY_H
