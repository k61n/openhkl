//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeExperiment.h
//! @brief     Defines class SubframeExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAMEEXPERIMENT_H
#define GUI_PANELS_SUBFRAMEEXPERIMENT_H

#include "gui/panels/SubframeImage.h"
#include "gui/panels/SubframeLogger.h"
#include "gui/panels/SubframePlot.h"
#include "gui/panels/SubframeProperties.h"
#include <QCR/widgets/views.h>

class SubframeExperiment : public QcrWidget {
 public:
    SubframeExperiment();

    ImageWidget* image;
    SubframeLogger* logger;
    SubframePlot* plot;
    SubframeSetup* properties;
};

#endif // GUI_PANELS_SUBFRAMEEXPERIMENT_H
