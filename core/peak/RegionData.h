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

class IntegrationRegion;

//! A small container class for visualising individual peaks mainly via Python interface
class RegionData {
 public:
    RegionData() = default;
    RegionData(
        IntegrationRegion* region, double xmin, double xmax, double ymin, double ymax,
        unsigned int zmin, unsigned int zmax);

    //! Add data from a single frame
    void addFrame(Eigen::MatrixXi& frame, Eigen::MatrixXi& mask);

    //! Return the frame data
    Eigen::MatrixXi frame(size_t i);
    //! Return the integration mask
    Eigen::MatrixXi mask(size_t i);
    //! Return the number of frames
    unsigned int nFrames() const;
    //! Return the index of the frame closest to the peak centre
    unsigned int centreFrame() const;

    IntegrationRegion* integrationRegion() const;
    double xmin() const;
    double xmax() const;
    double ymin() const;
    double ymax() const;
    unsigned int zmin() const;
    unsigned int zmax() const;

 private:
    IntegrationRegion* _integration_region;
    std::vector<Eigen::MatrixXi> _data;
    std::vector<Eigen::MatrixXi> _mask;

    double _xmin;
    double _xmax;
    double _ymin;
    double _ymax;
    unsigned int _zmin;
    unsigned int _zmax;
};

} // namespace nsx

#endif // NSX_CORE_PEAK_REGIONDATA_H
