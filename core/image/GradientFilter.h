//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/GradientFilter.h
//! @brief     Defines class GradientFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_GRADIENTFILTER_H
#define OHKL_CORE_IMAGE_GRADIENTFILTER_H

#include "core/image/ImageFilter.h"

#include <opencv2/opencv.hpp>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

enum class GradientDirection {X = 0, Y, Z};
enum class GradientFilterType { CentralDifference = 0, Sobel3 };
static const std::map<GradientFilterType, std::string> GradientFilterStrings {
    {GradientFilterType::CentralDifference, "Central difference"},
    {GradientFilterType::Sobel3, "Sobel 3x3"},
};

//! Pure virtual base class for Gradient filters

class GradientFilter : public ImageFilter {
 public:
    GradientFilter();

    void filter() override;
    virtual void filter(GradientDirection direction);

    void setXKernel(cv::Mat kernel_x) { _kernel_x = kernel_x; };
    void setYKernel(cv::Mat kernel_y) { _kernel_y = kernel_y; };

 protected:
    cv::Mat _kernel_x;
    cv::Mat _kernel_y;
};

} // namespace ohkl

#endif // OHKL_CORE_IMAGE_GRADIENTFILTER_H
