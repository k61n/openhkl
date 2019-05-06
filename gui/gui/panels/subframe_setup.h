
#ifndef SUBFRAME_SETUP_H
#define SUBFRAME_SETUP_H

#include <QTabWidget>
#include "gui/panels/tab_data.h"
#include "gui/panels/tab_unitcells.h"
#include "gui/panels/tab_peaks.h"
#include "gui/panels/tab_peaklibrary.h"
#include "gui/panels/tab_instrument.h"

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframeSetup : public QTabWidget {
public:
    SubframeSetup();
    TabData* data;
    TabUnitcells* unitcells;
    TabPeaklibrary* library;
    TabPeaks* peaks;
    TabInstrument* instrument;
};

#endif // SUBFRAME_SETUP_H
