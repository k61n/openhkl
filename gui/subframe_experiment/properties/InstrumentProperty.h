//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/properties/InstrumentProperty.h
//! @brief     Defines class TabInstrument
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_INSTRUMENTPROPERTY_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_INSTRUMENTPROPERTY_H

#include "gui/subframe_experiment/properties/DetectorProperty.h"
#include "gui/subframe_experiment/properties/SampleShapeProperties.h"
#include "gui/subframe_experiment/properties/SourceProperty.h"

//! Tab of the SubframeSetup that contains the detector, source and sample property tabs
class InstrumentProperty : public QTabWidget {
 public:
    InstrumentProperty();

    DetectorProperty* detector() { return _detector; };
    SourceProperty* monoSource() { return _monoSource; };
    SampleShapeProperties* sample() { return _sample; };

    void setCurrent(int);
    void refreshInput() const;

 private:
    DetectorProperty* _detector;
    SourceProperty* _monoSource;
    SampleShapeProperties* _sample;
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_INSTRUMENTPROPERTY_H
