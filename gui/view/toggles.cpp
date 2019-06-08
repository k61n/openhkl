//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/view/toggles.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/view/toggles.h"
#include "gui/mainwin.h"

Toggles::Toggles()
{
    pixelPosition.setHook([this](bool check) {
        if (check) {
            this->gammaNu.setCellValue(!check);
            this->twoTheta.setCellValue(!check);
            this->dSpacing.setCellValue(!check);
            this->millerIndices.setCellValue(!check);
        }
    });
    gammaNu.setHook([this](bool check) {
        if (check) {
            this->pixelPosition.setCellValue(!check);
            this->twoTheta.setCellValue(!check);
            this->dSpacing.setCellValue(!check);
            this->millerIndices.setCellValue(!check);
        }
    });
    twoTheta.setHook([this](bool check) {
        if (check) {
            this->pixelPosition.setCellValue(!check);
            this->gammaNu.setCellValue(!check);
            this->dSpacing.setCellValue(!check);
            this->millerIndices.setCellValue(!check);
        }
    });
    dSpacing.setHook([this](bool check) {
        if (check) {
            this->pixelPosition.setCellValue(!check);
            this->twoTheta.setCellValue(!check);
            this->gammaNu.setCellValue(!check);
            this->millerIndices.setCellValue(!check);
        }
    });
    millerIndices.setHook([this](bool check) {
        if (check) {
            this->pixelPosition.setCellValue(!check);
            this->twoTheta.setCellValue(!check);
            this->dSpacing.setCellValue(!check);
            this->gammaNu.setCellValue(!check);
        }
    });
    viewExperiment.setHook([](bool check) { gGui->dockExperiments_->setVisible(check); });
    viewImage.setHook([](bool check) { gGui->dockImage_->setVisible(check); });
    viewLogger.setHook([](bool check) { gGui->dockLogger_->setVisible(check); });
    viewPlotter.setHook([](bool check) { gGui->dockPlot_->setVisible(check); });
    viewProperties.setHook([](bool check) { gGui->dockProperties_->setVisible(check); });
}
