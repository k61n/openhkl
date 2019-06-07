//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/tab_instrument.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_TAB_INSTRUMENT_H
#define NSXGUI_GUI_PANELS_TAB_INSTRUMENT_H

#include "nsxgui/gui/panels/subtab_detector.h"
#include "nsxgui/gui/panels/subtab_monochromaticsource.h"
#include "nsxgui/gui/panels/subtab_sample.h"
#include <QCR/widgets/controls.h>

class TabInstrument : public QcrTabWidget {
public:
    TabInstrument();
    SubtabDetector* detector;
    SubtabMonochromatic* monoSource;
    SubtabSample* sample;

    void setCurrent(int);
};

#endif // NSXGUI_GUI_PANELS_TAB_INSTRUMENT_H
