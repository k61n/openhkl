
#ifndef NSXGUI_GUI_PANELS_SUBFRAME_SETUP_H
#define NSXGUI_GUI_PANELS_SUBFRAME_SETUP_H

#include "nsxgui/gui/panels/tab_data.h"
#include "nsxgui/gui/panels/tab_instrument.h"
#include "nsxgui/gui/panels/tab_peaklibrary.h"
#include "nsxgui/gui/panels/tab_peaks.h"
#include "nsxgui/gui/panels/tab_unitcells.h"
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
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_SETUP_H
