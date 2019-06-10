//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/tab_unitcells.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_PANELS_TAB_UNITCELLS_H
#define GUI_PANELS_TAB_UNITCELLS_H

#include "gui/properties/unitcellproperty.h"
//@ #include <QCR/widgets/views.h>

class TabUnitcells : public QcrWidget {
public:
    TabUnitcells();

private:
    UnitCellProperty* property;
};

#endif // GUI_PANELS_TAB_UNITCELLS_H
