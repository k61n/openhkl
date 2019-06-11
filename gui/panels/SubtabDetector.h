//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubtabDetector.h
//! @brief     Defines class SubtabDetector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBTABDETECTOR_H
#define GUI_PANELS_SUBTABDETECTOR_H

#include "gui/properties/DetectorProperty.h"

class SubtabDetector : public QcrWidget {
public:
    SubtabDetector();

private:
    DetectorProperty* property;
};

#endif // GUI_PANELS_SUBTABDETECTOR_H
