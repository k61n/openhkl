
#ifndef SUBFRAME_SETUP_H
#define SUBFRAME_SETUP_H

#include "nsxgui/qcr/widgets/controls.h"
#include "nsxgui/gui/panels/tab_data.h"
#include "nsxgui/gui/panels/tab_unitcells.h"
#include "nsxgui/gui/panels/tab_peaks.h"
#include "nsxgui/gui/panels/tab_peaklibrary.h"
#include "nsxgui/gui/panels/tab_instrument.h"

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

#endif // SUBFRAME_SETUP_H
