//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/SingleFrame.h
//! @brief     Defines class SingleFrame
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_DATA_SINGLEFRAME_H
#define OHKL_CORE_DATA_SINGLEFRAME_H

#include "core/data/DataSet.h"
#include "core/loader/IDataReader.h"

namespace ohkl {


/*! \addtogroup python_api
 *  @{*/

/*! \brief Special case of DataSet that contains only one image
 *
 *
 */
class SingleFrame : public DataSet {

 public:
    SingleFrame(const std::string& dataset_name, Diffractometer* diffractometer);

    //! Add a raw file to be read as a single detector image frame. Only allow one frame to be
    //! added.
    void addFrame(const std::string& rawfilename, DataFormat format);

    //! Read a single frame
    Eigen::MatrixXi frame(const std::size_t idx) const override;

    //! Returns frame after transforming to account for detector gain and baseline
    Eigen::MatrixXd transformedFrame(std::size_t idx) const override;

    //! Return per-pixel magnitude of gradient of a given frame
    Eigen::MatrixXd gradientFrame(std::size_t idx, GradientFilterType kernel) const override;

    void setNFrames(std::size_t nframes) override;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_DATA_SINGLEFRAME_H
