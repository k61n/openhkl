//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/GradientFilter.cpp
//! @brief     Implements class GradientFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/GradientFilter.h"
#include <opencv2/core/base.hpp>

namespace ohkl {

GradientFilter::GradientFilter() : ImageFilter({}) { }

void GradientFilter::filter()
{
    cv::Mat grad_x, grad_y, grad_x_sq, grad_y_sq;
    filter(GradientDirection::X);
    grad_x = _filtered_image;
    filter(GradientDirection::Y);
    grad_y = _filtered_image;

    grad_x_sq = grad_x.mul(grad_x);
    grad_y_sq = grad_y.mul(grad_y);
    cv::sqrt((grad_x_sq + grad_y_sq), _filtered_image);
}

void GradientFilter::filter(GradientDirection direction)
{
    const int ddepth = -1; // Same bit depth as original image
    const int delta = 0; // No baseline
    cv::Point anchor = {-1, -1}; // Anchor at centre of kernel

    switch (direction) {
        case (GradientDirection::X): {
            cv::filter2D(_image, _filtered_image, ddepth, _kernel_x, anchor, delta, cv::BORDER_REPLICATE);
            break;
        }
        case (GradientDirection::Y): {
            cv::filter2D(_image, _filtered_image, ddepth, _kernel_y, anchor, delta, cv::BORDER_REPLICATE);
            break;
        }
        default: {
            break;
        }
    }
}


} // namespace ohkl
