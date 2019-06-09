//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subtab_detector.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_PANELS_SUBTAB_DETECTOR_H
#define GUI_PANELS_SUBTAB_DETECTOR_H

#include "gui/properties/detectorproperty.h"
#include <QCR/widgets/views.h>

class SubtabDetector : public QcrWidget {
public:
    SubtabDetector();

private:
    DetectorProperty* property;
};

#endif // GUI_PANELS_SUBTAB_DETECTOR_H
