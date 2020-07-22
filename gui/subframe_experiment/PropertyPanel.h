//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/PropertyPanel.h
//! @brief     Defines class SubframeSetup
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTYPANEL_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTYPANEL_H

#include "gui/subframe_experiment/properties/InstrumentProperty.h"
#include "gui/subframe_experiment/properties/NumorProperty.h"
#include "gui/subframe_experiment/properties/PeakProperties.h"
#include "gui/subframe_experiment/properties/UnitCellProperty.h"

//! Part of the main window that contains the property tabs
class PropertyPanel : public QTabWidget {
 public:
    PropertyPanel();

    NumorProperty* _data;
    UnitCellProperty* _unitcells;
    PeakProperties* _peaks;
    InstrumentProperty* _instrument;

    void setCurrent(int);
    void dataChanged();
    void experimentChanged();
    void peaksChanged();
    void unitCellChanged();
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTYPANEL_H
