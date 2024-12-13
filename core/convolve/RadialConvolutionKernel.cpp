//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/RadialConvolutionKernel.cpp
//! @brief     Implements class RadialConvolutionKernel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/RadialConvolutionKernel.h"
#include "core/convolve/ConvolutionKernel.h"

#include <opencv2/core/hal/interface.h>
#include <stdexcept>

#include <iostream>

namespace ohkl {

RadialConvolutionKernel::RadialConvolutionKernel(double r_in, double r_out) : ConvolutionKernel()
{
    if (r_in < 0 || r_out < r_in)
        throw std::runtime_error(
            "RadialConvlutionKernel::RadialConvolutionKernel: invalid parameters");

    _size = 2 * r_out + 1;
    _matrix = cv::Mat::zeros(_size, _size, CV_64F);
    const int center = int(_size / 2);

    for (int i = 0; i < _size; ++i) {
        for (int j = 0; j < _size; ++j) {
            double x = i - center;
            double y = j - center;
            double dist2 = x * x + y * y;
            if (dist2 >= r_in * r_in && dist2 < r_out * r_out)
                _matrix.at<double>(i, j) = 1.0;
        }
    }

    _matrix /= cv::sum(_matrix)[0];
}


} // namespace ohkl
