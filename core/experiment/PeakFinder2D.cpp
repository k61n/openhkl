//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakFinder.cpp
//! @brief     Implements class PeakFinder
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/PeakFinder2D.h"

#include "base/utils/ProgressHandler.h"
#include "core/data/DataSet.h"
#include "core/image/FilterFactory.h"
#include "core/peak/Peak3D.h"

#include <Eigen/src/Core/Matrix.h>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <opencv2/imgcodecs.hpp>
#include <stdexcept>
#include <iostream>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

void PeakFinder2DParameters::log(const Level& level) const
{
    ohklLog(level, "2D Peak finder parameters:");
    ohklLog(level, "threshold              = ", threshold);
    ohklLog(level, "filter                 = ", static_cast<int>(kernel));
}

PeakFinder2D::PeakFinder2D() : _handler(nullptr), _current_data(nullptr)
{
    _params.minThreshold = 1;
    _params.maxThreshold = 100;
    _params.filterByCircularity = true;
    _params.filterByConvexity = false;
    _params.filterByInertia = false;
    _params.filterByArea = false;
    _params.filterByColor = false;
    _params.threshold = 30;
    _params.kernel = ImageFilterType::EnhancedAnnular;
}

void PeakFinder2D::setHandler(const sptrProgressHandler& handler)
{
    _handler = handler;
}

void PeakFinder2D::setData(sptrDataSet data)
{
    if (data != _current_data) {
        _current_data = data;
        _keypoint_collection.setData(data);
    }
}

void PeakFinder2D::find(std::size_t frame_idx)
{
    ohklLog(Level::Info, "PeakFinder2D::find: frame ", frame_idx);
    _params.log(Level::Info);
    FilterFactory factory;
    std::string filter_type = ImageFilterStrings.at(_params.kernel);
    ImageFilter* filter = factory.create(filter_type, _filter_params);

    RealMatrix frame = _current_data->frame(frame_idx).cast<double>();
    filter->setImage(frame);
    filter->filter();
    filter->threshold(_params.threshold, true);
    cv::Mat cv_frame_thresholded = filter->cvThresholdedImage();

    // SimpleBlobDetector only accepts 8 bit unsigned images
    cv::Mat cv_frame_8u;
    cv_frame_thresholded.convertTo(cv_frame_8u, CV_8U, 1.0);

    cv::imshow("thresholded", cv_frame_thresholded);
    cv::waitKey();

    _keypoint_collection.clearFrame(frame_idx);
    std::vector<cv::KeyPoint>* keypoints = _keypoint_collection.frame(frame_idx);
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(_params);
    detector->detect(cv_frame_8u, *keypoints);
    std::cout << keypoints->size() << " keypoints" << std::endl;
    ohklLog(Level::Info, "PeakFinder2D::find: found ", keypoints->size(), " blobs");
}

void PeakFinder2D::findAll()
{
    ohklLog(Level::Info, "PeakFinder2D::findAll: ", _current_data->nFrames(), " frames");
    // update progress handler
    if (_handler) {
        _handler->setStatus("Finding blobs");
        _handler->setProgress(0);
    }
    for (std::size_t idx = 0; idx < _current_data->nFrames(); ++idx) {
        find(idx);
        if (_handler)
            _handler->setProgress(100.0 * idx / _current_data->nFrames());
    }

    if (_handler)
        _handler->setProgress(100);

    ohklLog(Level::Info, "PeakFinder2D::findAll: done");
}

std::vector<Peak3D*> PeakFinder2D::getPeakList(std::size_t frame_index)
{
    if (!_current_data)
        return {};

    _found_peaks.clear();
    std::vector<Peak3D*> peaks;
    for (auto keypoint : *_keypoint_collection.frame(frame_index)) {
        Eigen::Vector3d center = {keypoint.pt.x, keypoint.pt.y, static_cast<double>(frame_index)};
        sptrPeak3D peak = std::make_shared<Peak3D>(_current_data);
        peak->setShape(Ellipsoid(center, 1.0));
        _found_peaks.emplace_back(peak);
        peaks.push_back(peak.get());
    }
    std::map<Peak3D*, ohkl::RejectionFlag> tmp_map;
    _current_data->maskPeaks(peaks, tmp_map);
    return peaks;
}

} // namespace ohkl
