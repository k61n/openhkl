//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/AnnularImageFilter.h
//! @brief     Defines class AnnularImageFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_ANNULARIMAGEFILTER_H
#define OHKL_CORE_IMAGE_ANNULARIMAGEFILTER_H

#include "core/image/ImageFilter.h"

#include "core/convolve/RadialConvolutionKernel.h"

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

class AnnularImageFilter : public ImageFilter {
 public:
    AnnularImageFilter(double r1, double r2, double r3);
    void filter() override;

 protected:
    RadialConvolutionKernel _peak_kernel;
    RadialConvolutionKernel _bkg_kernel;
};

} // namespace ohkl

#endif // OHKL_CORE_IMAGE_ANNULARIMAGEFILTER_H
