//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_setup.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_SUBFRAME_SETUP_H
#define NSXGUI_GUI_PANELS_SUBFRAME_SETUP_H

#include "gui/panels/tab_data.h"
#include "gui/panels/tab_instrument.h"
#include "gui/panels/tab_peaklibrary.h"
#include "gui/panels/tab_peaks.h"
#include "gui/panels/tab_unitcells.h"
#include <QCR/widgets/controls.h>

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframeSetup : public QcrTabWidget {
public:
    SubframeSetup();
    TabData* data;
    TabUnitcells* unitcells;
    TabPeaklibrary* library;
    TabPeaks* peaks;
    TabInstrument* instrument;

    void setCurrent(int);
    void dataChanged();
    void experimentChanged();
    void peaksChanged();
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_SETUP_H
