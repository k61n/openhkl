//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeSetup.h
//! @brief     Defines class SubframeSetup
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAMESETUP_H
#define GUI_PANELS_SUBFRAMESETUP_H

#include "gui/panels/TabInstrument.h"
#include "gui/panels/TabPeakLibrary.h"
#include "gui/panels/TabPeaks.h"
#include "gui/properties/UnitCellProperty.h"
#include "gui/properties/NumorProperty.h"

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframeSetup : public QcrTabWidget {
public:
    SubframeSetup();
    NumorProperty* data;
    UnitCellProperty* unitcells;
    TabPeaklibrary* library;
    TabPeaks* peaks;
    TabInstrument* instrument;

    void setCurrent(int);
    void dataChanged();
    void experimentChanged();
    void peaksChanged();
};

#endif // GUI_PANELS_SUBFRAMESETUP_H
