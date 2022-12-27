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

#include "core/convolve/Convolver.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/shape/KeyPointCollection.h"
#include "core/shape/PeakCollection.h"

#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

/*! \brief Perform image recognition on detector images to find peaks in 2D
 *
 *  PLACEHOLDER
 */

struct PeakFinder2DParameters : public cv::SimpleBlobDetector::Params {
    ConvolutionKernelType kernel = ConvolutionKernelType::Annular; //!< Convolution kernel type
    int threshold = 80; //!< Threshold for image thresholding (post-filter)

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

    //! Set the convolver
    void setConvolver(const ConvolutionKernelType& kernel);
    //! Get the convolver
    Convolver* convolver() const { return _convolver.get(); };

    //! Generate a list of peaks from found blobs
    std::vector<Peak3D*> getPeakList(std::size_t frame_index);

    //! Determine whether we have peaks for a specific frame index
    bool hasPeaks(std::size_t frame_idx);

 private:
    //! progress handler
    sptrProgressHandler _handler;
    //! The DataSet
    sptrDataSet _current_data;
    //! Convolver for image filtering
    std::unique_ptr<Convolver> _convolver;

    //! Blob detection parameters
    PeakFinder2DParameters _params;

    //! Vector of keypoints per frame
    KeyPointCollection _keypoint_collection;

    //! temporary list of peaks for indexing
    PeakList _found_peaks;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_PEAKFINDER2D_H
