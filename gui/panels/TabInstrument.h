//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/TabInstrument.h
//! @brief     Defines class TabInstrument
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_TABINSTRUMENT_H
#define GUI_PANELS_TABINSTRUMENT_H

#include "gui/panels/SubtabDetector.h"
#include "gui/panels/SubtabMonochromaticSource.h"
#include "gui/panels/SubtabSample.h"

class TabInstrument : public QcrTabWidget {
public:
    TabInstrument();
    SubtabDetector* detector;
    SubtabMonochromatic* monoSource;
    SubtabSample* sample;

    void setCurrent(int);
};

#endif // GUI_PANELS_TABINSTRUMENT_H
