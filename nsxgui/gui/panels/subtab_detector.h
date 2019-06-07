//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subtab_detector.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_SUBTAB_DETECTOR_H
#define NSXGUI_GUI_PANELS_SUBTAB_DETECTOR_H

#include "nsxgui/gui/properties/detectorproperty.h"
#include <QCR/widgets/views.h>

class SubtabDetector : public QcrWidget {
public:
    SubtabDetector();

private:
    DetectorProperty* property;
};

#endif // NSXGUI_GUI_PANELS_SUBTAB_DETECTOR_H
