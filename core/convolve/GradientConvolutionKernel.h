//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/GradientConvolutionKernel.h
//! @brief     Defines class GradientConvolutionKernel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_GRADIENTCONVOLUTIONKERNEL_H
#define OHKL_CORE_CONVOLVE_GRADIENTCONVOLUTIONKERNEL_H

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

//! Pure virtual base class for RadialGradientConvolutionKernel

class GradientConvolutionKernel {
 public:
    GradientConvolutionKernel();

    const cv::Mat& matrixX() const { return _matrix_x; };
    const cv::Mat& matrixY() const { return _matrix_y; };

 protected:
    cv::Mat _matrix_x;
    cv::Mat _matrix_y;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_GRADIENTCONVOLUTIONKERNEL_H
