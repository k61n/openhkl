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

#include "core/convolve/Convolver.h"
#include "core/data/ImageGradient.h"

//! Which mode is the cursor diplaying
enum TooltipMode { Cursor = 0, Pixel = 1, Theta = 2, GammaNu = 3, DSpacing = 4, MillerIndices = 5 };

//! Container for toggling elements of DetectorSceneParams
struct DetectorSceneParams {
    // toggles
    bool logarithmic = false;
    bool gradient = false;
    bool fftGradient = false;
    bool integrationRegion = false;
    bool singlePeakIntRegion = false;
    bool filteredImage = false;
    bool directBeam = false;
    bool peaks = true;
    bool extPeaks = false;
    bool detectorSpots = false;
    bool labels = false;
    bool masks = true;
    bool contours = false;

    // values
    int currentIndex = -1;
    int intensity = 3000;

    // image convolution
    double threshold = 80;
    std::map<std::string, double> convolver_params = {
        {"r1", 5.0}, {"r2", 10.0}, {"r3", 15.0}, {"r_in", 5.0}, {"r_out", 10.0}, {"box_size", 3.0},
    };
    ohkl::ConvolutionKernelType convolver = ohkl::ConvolutionKernelType::Annular;
    ohkl::GradientKernel gradientKernel = ohkl::GradientKernel::Sobel;
    TooltipMode tooltipMode = Pixel;

    // contours
    unsigned int n_contours = 0;
    double d_min = 1.5;
    double d_max = 50.0;
};

#endif // OHKL_GUI_GRAPHICS_DETECTORSCENEPARAMS_H
