//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subtab_sample.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBTAB_SAMPLE_H
#define GUI_PANELS_SUBTAB_SAMPLE_H

#include "gui/properties/sampleshapeproperties.h"

class SubtabSample : public QcrWidget {
public:
    SubtabSample();

private:
    SampleShapeProperties* property;
};

#endif // GUI_PANELS_SUBTAB_SAMPLE_H
