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
#include "core/data/DataTypes.h"

#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>

#include <opencv2/imgcodecs.hpp>

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

void PeakFinder2D::find(std::size_t frame_idx)
{
    Eigen::MatrixXi frame = _current_data->frame(frame_idx);
    cv::Mat cv_frame, cv_frame_8u;
    cv::eigen2cv(frame, cv_frame);

    // SimpleBlobDetector only accepts 8 bit unsigned images
    double scale = 1.0 / 256.0;
    cv_frame.convertTo(cv_frame_8u, CV_8U, scale);

    _per_frame_spots[frame_idx].clear();
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(_params);
    detector->detect(cv_frame_8u, _per_frame_spots[frame_idx]);
}

} // namespace ohkl
