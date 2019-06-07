//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/properties/numorproperty.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PROPERTIES_NUMORPROPERTY_H
#define NSXGUI_GUI_PROPERTIES_NUMORPROPERTY_H

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

#endif // NSXGUI_GUI_PROPERTIES_NUMORPROPERTY_H
