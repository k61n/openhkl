//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakFinder2D.h
//! @brief     Defines class PeakFinder2D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_PEAKFINDER2D_H
#define OHKL_CORE_EXPERIMENT_PEAKFINDER2D_H

#include "base/utils/LogLevel.h"
#include "core/data/DataTypes.h"
#include "core/image/ImageFilter.h"
#include "core/shape/KeyPointCollection.h"

#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

class Peak3D;
class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

/*! \brief Perform image recognition on detector images to find peaks in 2D
 *
 *  PLACEHOLDER
 */

struct PeakFinder2DParameters : public cv::SimpleBlobDetector::Params {
    ImageFilterType kernel = ImageFilterType::EnhancedAnnular; //!< Convolution kernel type
    int threshold = 80; //!< Threshold for image thresholding (post-filter)
    double r1 = 5.0; //!< Upper bound for positive region of filter kernel
    double r2 = 10.0; //!< Lower bound for negative region of filter kernel
    double r3 = 15.0; //!< Upper bound for negative region of filter kernel

    void log(const Level& level) const;
};

class PeakFinder2D {
 public:
    PeakFinder2D();

    //! Find blobs on given image
    void find(std::size_t image_idx);
    //! Find blobs for all images in the data set
    void findAll();

    //! Set the progress handler
    void setHandler(const sptrProgressHandler& handler);
    //! Set the DataSet
    void setData(sptrDataSet data);
    //! Return the DataSet
    sptrDataSet currentData() const { return _current_data; };

    //! Get the parameters
    PeakFinder2DParameters* parameters() { return &_params; };

    //! Get object storing per-frame keypoints
    KeyPointCollection* keypoints() { return &_keypoint_collection; };

    //! Generate a list of peaks from found blobs
    std::vector<Peak3D*> getPeakList(std::size_t frame_index);

    //! Set image filter parameters
    void setFilterParameters(const std::map<std::string, double>& params);

    //! Get the image filter parameters
    std::map<std::string, double> filterParameters();

 private:
    //! progress handler
    sptrProgressHandler _handler;
    //! The DataSet
    sptrDataSet _current_data;
    //! Blob detection parameters
    PeakFinder2DParameters _params;
    //! Vector of keypoints per frame
    KeyPointCollection _keypoint_collection;
    //! temporary list of peaks for indexing
    PeakList _found_peaks;
    //! The parameters for the image filter
    std::map<std::string, double> _filter_params;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_PEAKFINDER2D_H
