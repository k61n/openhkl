//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/TabUnitCells.h
//! @brief     Defines class TabUnitcells
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_TABUNITCELLS_H
#define GUI_PANELS_TABUNITCELLS_H

#include "gui/properties/UnitCellProperty.h"

class TabUnitcells : public QcrWidget {
public:
    TabUnitcells();

private:
    UnitCellProperty* property;
};

#endif // GUI_PANELS_TABUNITCELLS_H
