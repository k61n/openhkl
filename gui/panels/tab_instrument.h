//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/tab_instrument.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_PANELS_TAB_INSTRUMENT_H
#define GUI_PANELS_TAB_INSTRUMENT_H

#include "gui/panels/subtab_detector.h"
#include "gui/panels/subtab_monochromaticsource.h"
#include "gui/panels/subtab_sample.h"
class TabInstrument : public QcrTabWidget {
public:
    TabInstrument();
    SubtabDetector* detector;
    SubtabMonochromatic* monoSource;
    SubtabSample* sample;

    void setCurrent(int);
};

#endif // GUI_PANELS_TAB_INSTRUMENT_H
