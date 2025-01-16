//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/Sobel3Filter.cpp
//! @brief     Implements class Sobel3Filter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/Sobel3Filter.h"

#include "core/convolve/CentralDifferenceKernel.h"
#include "core/image/GradientFilter.h"
#include <opencv2/core/base.hpp>
#include <opencv2/opencv.hpp>

namespace ohkl {

Sobel3Filter::Sobel3Filter() : GradientFilter()
{
}

void Sobel3Filter::filter(GradientDirection direction)
{
    const int ddepth = -1; // Same bit depth as original image
    const int delta = 0; // No baseline
    const int ksize = 3;
    const int scale = 1;

    switch (direction) {
        case (GradientDirection::X): {
            cv::Sobel(_image, _filtered_image, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_REPLICATE);
            break;
        }
        case (GradientDirection::Y): {
            cv::Sobel(_image, _filtered_image, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_REPLICATE);
            break;
        }
        default: {
            break;
        }
    }
}

void Sobel3Filter::filter()
{
    GradientFilter::filter();
}


} // namespace ohkl
