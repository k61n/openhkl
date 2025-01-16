//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/CentralDifferenceKernel.cpp
//! @brief     Implements class CentralDifferenceKernel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/CentralDifferenceKernel.h"

#include <opencv2/core/hal/interface.h>
#include <stdexcept>

namespace ohkl {

CentralDifferenceKernel::CentralDifferenceKernel() : GradientConvolutionKernel()
{
    _matrix_x = cv::Mat::zeros(3, 3, CV_64F);
    _matrix_x.at<double>(1, 0) = -1;
    _matrix_x.at<double>(1, 2) = 1;
    _matrix_y = cv::Mat::zeros(3, 3, CV_64F);
    _matrix_y.at<double>(0, 1) = -1;
    _matrix_y.at<double>(2, 1) = 1;
    _matrix_x /= 2;
    _matrix_y /= 2;
}

} // namespace ohkl
