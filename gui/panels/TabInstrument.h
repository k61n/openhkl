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

#include "gui/subframe_experiment/properties/DetectorProperty.h"
#include "gui/subframe_experiment/properties/SampleShapeProperties.h"
#include "gui/subframe_experiment/properties/SourceProperty.h"

//! Tab of the SubframeSetup that contains the detector, source and sample property tabs
class TabInstrument : public QcrTabWidget {
 public:
    TabInstrument();
    DetectorProperty* detector;
    SourceProperty* monoSource;
    SampleShapeProperties* sample;

    void setCurrent(int);
};

#endif // GUI_PANELS_TABINSTRUMENT_H
