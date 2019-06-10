//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/numorproperty.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PROPERTIES_NUMORPROPERTY_H
#define GUI_PROPERTIES_NUMORPROPERTY_H

#include <QCR/widgets/views.h>
#include <QTableWidget>

class NumorProperty : public QcrWidget {
public:
    NumorProperty();

private:
    void onChanged();
    void onRemake();
    void clear();
    QTableWidget* table;
    QLabel* label;
};

#endif // GUI_PROPERTIES_NUMORPROPERTY_H
