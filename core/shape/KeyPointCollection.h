//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/KeyPointCollection.h
//! @brief     defines class KeyPointCollection
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_KEYPOINTCOLLECTION_H
#define OHKL_CORE_SHAPE_KEYPOINTCOLLECTION_H

#include "core/data/DataTypes.h"

#include <opencv2/core/types.hpp>

#include <vector>

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Store a collection of OpenCV keypoints
 *
 * Used by PeakFinder2D to store blob centres, no shape information
 */
class KeyPointCollection {
 public:
    KeyPointCollection();

    void setData(sptrDataSet data);
    std::vector<cv::KeyPoint>* frame(std::size_t idx);
    void clearFrame(std::size_t idx);
    bool hasPeaks(std::size_t idx);
 private:
    sptrDataSet _data;
    std::vector<std::unique_ptr<std::vector<cv::KeyPoint>>> _keypoint_frames;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_KEYPOINTCOLLECTION_H
