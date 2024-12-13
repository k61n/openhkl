//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/AnnularImageFilter.cpp
//! @brief     Implements class AnnularImageFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/AnnularImageFilter.h"

#include "core/convolve/Convolver.h"

#include <opencv2/core/base.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

namespace ohkl {

AnnularImageFilter::AnnularImageFilter(double r1, double r2, double r3)
    : ImageFilter(), _peak_kernel(0.0, r1), _bkg_kernel(r2, r3)
{
    _size = static_cast<int>(2 * r3 + 1);
}

void AnnularImageFilter::filter()
{
    const cv::Point anchor(-1, -1);
    const int ddepth = CV_64F; // Output image type same as input
    const int delta = 0; //
    cv::Mat peak_image, bkg_image, filtered_image;

    cv::filter2D(
        _image, peak_image, ddepth, _peak_kernel.matrix(), anchor, delta,
        cv::BorderTypes::BORDER_WRAP);
    cv::filter2D(
        _image, bkg_image, ddepth, _bkg_kernel.matrix(), anchor, delta,
        cv::BorderTypes::BORDER_WRAP);
    cv::subtract(peak_image, bkg_image, _filtered_image);
}

} // namespace ohkl
