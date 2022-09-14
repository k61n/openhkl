//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakFinder.cpp
//! @brief     Implements class PeakFinder
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/PeakFinder2D.h"

#include "core/convolve/Convolver.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataTypes.h"

#include <Eigen/src/Core/Matrix.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>

#include <opencv2/imgcodecs.hpp>
#include <stdexcept>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

PeakFinder2D::PeakFinder2D()
{
    _params.minThreshold = 1;
    _params.maxThreshold = 100;
    _params.filterByCircularity = false;
    _params.filterByConvexity = false;
    _params.filterByInertia = false;
    _params.filterByArea = false;
    _params.filterByColor = false;
    _params.threshold = 80;
    _params.kernel = ConvolutionKernelType::Annular;

    setConvolver(_params.kernel);
}

void PeakFinder2D::setData(sptrDataSet data)
{
    _current_data = data;
    if (data != _current_data) {
        for (auto vec : _per_frame_spots)
            vec.clear();
        _per_frame_spots.clear();
    }
    for (std::size_t i = 0; i < data->nFrames(); ++i) {
        std::vector<cv::KeyPoint> keypoints;
        _per_frame_spots.push_back(keypoints);
    }
}

void PeakFinder2D::setConvolver(const ConvolutionKernelType& kernel)
{
    auto kernel_type = Convolver::kernelTypes.find(kernel);
    _convolver.reset(ConvolverFactory{}.create(kernel_type->second, {}));
}

void PeakFinder2D::find(std::size_t frame_idx)
{
    setConvolver(_params.kernel);
    RealMatrix frame = _current_data->frame(frame_idx).cast<double>();
    RealMatrix filtered_frame = _convolver->convolve(frame);

    cv::Mat cv_frame, cv_frame_thresholded, cv_frame_8u;
    cv::eigen2cv(filtered_frame, cv_frame);
    //! Maximum count determined by bit depth
    double max_count = pow(2, static_cast<int>(_current_data->bitDepth()));
    cv::threshold(
        cv_frame, cv_frame_thresholded, _params.threshold, static_cast<int>(max_count),
        cv::THRESH_BINARY_INV);
    // SimpleBlobDetector only accepts 8 bit unsigned images
    double fac = 1.0 / 256.0;
    double scale = 1.0;
    switch (_current_data->bitDepth()) {
        case BitDepth::u8b: {
            break;
        }
        case BitDepth::u16b: {
            scale = fac;
            break;
        }
        case BitDepth::u32b: {
            scale = fac * fac;
            break;
        }
        default: {
            throw std::runtime_error("PeakFinder2D::find: unexpected image bit depth");
        }
    }
    cv_frame_thresholded.convertTo(cv_frame_8u, CV_8U, scale);

    _per_frame_spots[frame_idx].clear();
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(_params);
    detector->detect(cv_frame_8u, _per_frame_spots[frame_idx]);
}

} // namespace ohkl
