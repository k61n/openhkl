//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/ImageFilter.cpp
//! @brief     Implements class ImageFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/ImageFilter.h"

#include "core/convolve/Convolver.h"

#include <opencv2/core/eigen.hpp>

namespace ohkl {

ImageFilter::ImageFilter(const FilterParameters& params) : _kernel(), _parameters(params)
{
}

void ImageFilter::setKernel(const cv::Mat& kernel)
{
    _kernel = kernel;
}

void ImageFilter::setImage(const RealMatrix& image)
{
    cv::eigen2cv(image, _image);
    _image_rows = image.rows();
    _image_cols = image.cols();
}

void ImageFilter::filter()
{
    const int ddepth = -1; // Same bit depth as original image
    const int delta = 0; // No baseline
    cv::Point anchor = {-1, -1}; // Anchor at centre of kernel

    cv::filter2D(_image, _filtered_image, ddepth, _kernel, anchor, delta);
}

void ImageFilter::threshold(double thresh)
{
    cv::threshold(_filtered_image, _thresholded_image, thresh, 1, cv::THRESH_BINARY);
}

RealMatrix ImageFilter::filteredImage()
{
    RealMatrix filtered_image(_image_rows, _image_cols);
    cv::cv2eigen(_filtered_image, filtered_image);
    return filtered_image;
}

RealMatrix ImageFilter::thresholdedImage()
{
    RealMatrix thresholded_image(_image_rows, _image_cols);
    cv::cv2eigen(_thresholded_image, thresholded_image);
    return thresholded_image;
}

} // namespace ohkl
