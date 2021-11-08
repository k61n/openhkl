//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/RegionData.h
//! @brief     Defines class RegionData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_PEAK_REGIONDATA_H
#define NSX_CORE_PEAK_REGIONDATA_H

#include <Eigen/Dense>
#include <vector>

namespace nsx {

//! A small container class for visualising individual peaks mainly via Python interface
class RegionData {
 public:
    RegionData() = default;

    //! Add data from a single frame
    void addFrame(Eigen::MatrixXi& frame, Eigen::MatrixXi& mask);

    //! Return the frame data
    Eigen::MatrixXi frame(size_t i);
    //! Return the integration mask
    Eigen::MatrixXi mask(size_t i);
    //! Return the number of frames
    int nFrames() const;

 private:
    std::vector<Eigen::MatrixXi> _data;
    std::vector<Eigen::MatrixXi> _mask;
};

} // namespace nsx

#endif // NSX_CORE_PEAK_REGIONDATA_H
