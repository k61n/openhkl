//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/RegionData.h
//! @brief     Defines class RegionData
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_PEAK_REGIONDATA_H
#define OHKL_CORE_PEAK_REGIONDATA_H

#include "core/shape/Profile3D.h"

#include <Eigen/Dense>
#include <vector>

namespace ohkl {

class IntegrationRegion;
class ShapeModel;

//! A small container class for visualising individual peaks mainly via Python interface
class RegionData {
 public:
    RegionData() = default;
    RegionData(
        IntegrationRegion* region, unsigned int xmin, unsigned int xmax, unsigned int ymin,
        unsigned int ymax, unsigned int zmin, unsigned int zmax);

    //! Add data from a single frame
    void addFrame(unsigned int frame_index, Eigen::MatrixXi& frame, Eigen::MatrixXi& mask);

    //! Return the frame index data
    int index(std::size_t i);
    //! Return the frame data
    Eigen::MatrixXi frame(std::size_t i);
    //! Return the profile data
    Eigen::MatrixXd profileData(std::size_t i);
    //! Return the profile
    Profile3D* profile();
    //! Return the integration mask
    Eigen::MatrixXi mask(std::size_t i);
    //! Return the number of frames
    unsigned int nFrames() const;
    //! Return the index of the frame closest to the peak centre
    unsigned int centreFrame() const;
    //! Get the index for the given frame in the RegionData containers
    unsigned int getRegionDataIndex(unsigned int frame_index);
    //! Compute a profile given a peak and shape model
    void buildProfile(ShapeModel* shapes, double radius, double nframes);
    //! Scale the profile by the intensity
    void scaleProfile();
    //! Return the maximum value of the peak data
    double dataMax() const;
    //! Return the maximum value of the profile data
    double profileMax() const;
    //! Return the number of profiles used in the mean profile
    int nProfiles() const { return _n_profiles; };

    IntegrationRegion* integrationRegion() const;
    unsigned int xmin() const;
    unsigned int xmax() const;
    unsigned int ymin() const;
    unsigned int ymax() const;
    unsigned int zmin() const;
    unsigned int zmax() const;
    unsigned int cols() const;
    unsigned int rows() const;


 private:
    IntegrationRegion* _integration_region;
    std::vector<Eigen::MatrixXi> _data;
    std::vector<Eigen::MatrixXi> _mask;
    std::vector<Eigen::MatrixXd> _profile_data;
    Profile3D _profile;
    std::vector<int> _index;

    int _n_profiles;

    unsigned int _xmin;
    unsigned int _xmax;
    unsigned int _ymin;
    unsigned int _ymax;
    unsigned int _zmin;
    unsigned int _zmax;
};

} // namespace ohkl

#endif // OHKL_CORE_PEAK_REGIONDATA_H
