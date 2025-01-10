//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/EnhancedAnnularImageFilter.cpp
//! @brief     Implements class EnhancedAnnularImageFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/EnhancedAnnularImageFilter.h"

#include "core/image/AnnularImageFilter.h"

#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/opencv.hpp>

namespace ohkl {

EnhancedAnnularImageFilter::EnhancedAnnularImageFilter(const FilterParameters& params)
    : AnnularImageFilter(params)
{
}

void EnhancedAnnularImageFilter::filter()
{
    const cv::Point anchor(-1, -1);
    const int ddepth = -1; // Output image type same as input
    const int delta = 0;   // 
    cv::Mat peak_image, bkg_image, diff, diff2, std;

    cv::filter2D(
        _image, peak_image, ddepth, _peak_kernel.matrix(), anchor, delta,
        cv::BorderTypes::BORDER_WRAP);
    cv::filter2D(
        _image, bkg_image, ddepth, _bkg_kernel.matrix(), anchor, delta,
        cv::BorderTypes::BORDER_WRAP);
    diff2 = (_image - bkg_image).mul(_image - bkg_image);
    cv::sqrt(diff2, diff);
    cv::filter2D(
        diff, std, ddepth, _bkg_kernel.matrix(), anchor, delta,
        cv::BorderTypes::BORDER_WRAP);
    cv::subtract(peak_image, bkg_image, _filtered_image);
    _filtered_image /= std;
}

} // namespace ohkl
