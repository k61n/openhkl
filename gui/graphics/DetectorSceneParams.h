//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/DetectorSceneParams.h
//! @brief     Defines class DetectorSceneParams
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_DETECTORSCENEPARAMS_H
#define OHKL_GUI_GRAPHICS_DETECTORSCENEPARAMS_H

#include "core/data/ImageGradient.h"

//! Container for toggling elements of DetectorSceneParams
struct DetectorSceneParams {
    // toggles
    bool logarithmic = false;
    bool gradient = false;
    bool fft_gradient = false;
    bool integrationRegion = false;
    bool singlePeakIntRegion = false;
    bool directBeam = false;
    bool extPeaks = false;
    bool detectorSpots = false;
    bool masks = true;

    // values
    int intensity = 3000;
    ohkl::GradientKernel gradient_kernel = ohkl::GradientKernel::Sobel;
};

#endif // OHKL_GUI_GRAPHICS_DETECTORSCENEPARAMS_H
