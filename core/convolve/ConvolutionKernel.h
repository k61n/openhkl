//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/ConvolutionKernel.h
//! @brief     Defines class ConvolutionKernel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_CONVOLUTIONKERNEL_H
#define OHKL_CORE_CONVOLVE_CONVOLUTIONKERNEL_H

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

//! Pure virtual base class for RadialConvolutionKernel

class ConvolutionKernel {
 public:
    ConvolutionKernel();

    const cv::Mat& matrix() const { return _matrix; };

 protected:
    cv::Mat _matrix;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_CONVOLUTIONKERNEL_H
