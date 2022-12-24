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

//! Which mode is the cursor diplaying
enum TooltipMode {
    Cursor = 0,
    Pixel = 1,
    Theta = 2,
    GammaNu = 3,
    DSpacing = 4,
    MillerIndices = 5
};

//! Container for toggling elements of DetectorSceneParams
struct DetectorSceneParams {
    // toggles
    bool logarithmic = false;
    bool gradient = false;
    bool fftGradient = false;
    bool integrationRegion = false;
    bool singlePeakIntRegion = false;
    bool directBeam = false;
    bool extPeaks = false;
    bool detectorSpots = false;
    bool masks = true;

    // values
    int currentIndex = -1;
    int intensity = 3000;
    ohkl::GradientKernel gradientKernel = ohkl::GradientKernel::Sobel;
    TooltipMode tooltipMode = Pixel;
};

#endif // OHKL_GUI_GRAPHICS_DETECTORSCENEPARAMS_H
