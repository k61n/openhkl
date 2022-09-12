//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakFinder2D.h
//! @brief     Defines class PeakFinder2D
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_PEAKFINDER2D_H
#define OHKL_CORE_EXPERIMENT_PEAKFINDER2D_H

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/shape/PeakCollection.h"

#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

class ProgressHandler;

/*! \brief Perform image recognition on detector images to find peaks in 2D
 *
 *  PLACEHOLDER
 */

class PeakFinder2D {
 public:
    PeakFinder2D();

    //! Find blobs on given image
    void find(std::size_t image_idx);

    //! Set the DataSet
    void setData(sptrDataSet data);
    //! Return the DataSet
    sptrDataSet currentData() const { return _current_data; };

    //! Get the parameters
    cv::SimpleBlobDetector::Params* parameters() { return &_params; };

    //! Get the vector of vectors of keypoints
    std::vector<std::vector<cv::KeyPoint>>* keypoints() { return &_per_frame_spots; };

    //! Set the progress handler
    void setHandler(const sptrProgressHandler& handler) { _handler = handler; };

 private:

 private:
    sptrProgressHandler _handler;

    //! The DataSet
    sptrDataSet _current_data;

    //! Blob detection parameters
    cv::SimpleBlobDetector::Params _params;

    //! Vector of keypoints per frame
    std::vector<std::vector<cv::KeyPoint>> _per_frame_spots;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_PEAKFINDER2D_H
