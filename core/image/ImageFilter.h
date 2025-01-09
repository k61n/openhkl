//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/ImageFilter.h
//! @brief     Defines class ImageFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_IMAGEFILTER_H
#define OHKL_CORE_IMAGE_IMAGEFILTER_H

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

namespace ohkl {

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

using FilterParameters = std::map<std::string, double>;

enum class ImageFilterType { Annular = 0, EnhancedAnnular };
static const std::map<ImageFilterType, std::string> ImageFilterStrings {
    {ImageFilterType::Annular, "Annular"},
    {ImageFilterType::EnhancedAnnular, "Enhanced annular"},
};

//! Pure virtual base class for ConstantConvolver, DeltaConvolver, RadialConvolver.

class ImageFilter {
 public:
    ImageFilter(const FilterParameters& params);
    virtual ~ImageFilter() = default;

    void setKernel(const cv::Mat& kernel);
    void setImage(const RealMatrix& image);
    virtual void filter();
    virtual void threshold(double thresh);

    virtual int kernelSize() const { return _kernel.rows; }

    RealMatrix filteredImage();
    RealMatrix thresholdedImage();
    cv::Mat cvFilteredImage() { return _filtered_image; };
    cv::Mat cvThresholdedImage() { return _thresholded_image; };

 protected:
    cv::Mat _kernel;
    std::size_t _size;

    std::size_t _image_rows;
    std::size_t _image_cols;

    cv::Mat _image;
    cv::Mat _filtered_image;
    cv::Mat _thresholded_image;

    FilterParameters _parameters;
};

} // namespace ohkl

#endif // OHKL_CORE_IMAGE_IMAGEFILTER_H
