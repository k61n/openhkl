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

#include "core/convolve/Convolver.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataTypes.h"
#include "core/peak/Peak3D.h"

#include <Eigen/src/Core/Matrix.h>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>

#include <opencv2/imgcodecs.hpp>
#include <stdexcept>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

void PeakFinder2DParameters::log(const Level& level) const
{
    ohklLog(level, "2D Peak finder parameters:");
    ohklLog(level, "threshold              = ", threshold);
    ohklLog(level, "convolver              = ", static_cast<int>(kernel));
}

PeakFinder2D::PeakFinder2D() : _handler(nullptr)
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

void PeakFinder2D::setHandler(const sptrProgressHandler& handler)
{
    _handler = handler;
}

void PeakFinder2D::setData(sptrDataSet data)
{
    if (data != _current_data) {
        _current_data = data;
        for (auto vec : _per_frame_spots)
            vec.clear();
        _per_frame_spots.clear();
        for (std::size_t i = 0; i < data->nFrames(); ++i) {
            std::vector<cv::KeyPoint> keypoints;
            _per_frame_spots.push_back(keypoints);
        }
    }
}

void PeakFinder2D::setConvolver(const ConvolutionKernelType& kernel)
{
    auto kernel_type = Convolver::kernelTypes.find(kernel);
    _convolver.reset(ConvolverFactory{}.create(kernel_type->second, {}));
}

void PeakFinder2D::find(std::size_t frame_idx)
{
    ohklLog(Level::Info, "PeakFinder2D::find: frame ", frame_idx);
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
    ohklLog(
        Level::Info, "PeakFinder2D::find: found ", _per_frame_spots[frame_idx].size(), " blobs");
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
    _found_peaks.clear();
    std::vector<Peak3D*> peaks;
    for (auto keypoint : _per_frame_spots.at(frame_index)) {
        Eigen::Vector3d center = {keypoint.pt.x, keypoint.pt.y, (double)frame_index};
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
