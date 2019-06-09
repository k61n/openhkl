//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/ShapeLibrary.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INTEGRATION_SHAPELIBRARY_H
#define CORE_INTEGRATION_SHAPELIBRARY_H

#include <Eigen/Dense>
#include <array>
#include <map>
#include <vector>

#include "core/crystal/MillerIndex.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/CrystalTypes.h"
#include "core/experiment/DataTypes.h"
#include "core/integration/Profile1D.h"
#include "core/integration/Profile3D.h"

namespace nsx {

enum class PeakInterpolation { NoInterpolation = 0, InverseDistance = 1, Intensity = 2 };

struct FitData;

//! Helper function for predicting peaks
PeakList predictPeaks(
    ShapeLibrary library, sptrDataSet data, sptrUnitCell, double dmin, double dmax, double radius,
    double nframes, int min_neighbors, PeakInterpolation interpolation);

//! \brief Class to store a library of peak shapes, to be used for peak
//! prediction and integration.
//!
//! The library stores a list of reference peaks. For each reference peak, the
//! library stores the covariance matrix of the intensity distribution, as well
//! as 3d- and 1d- integrated profiles. The covariance matrices are used to
//! roughly predict the shapes of weak peaks, and the integrated profiles are
//! used in the profile-fitting integration methods.
class ShapeLibrary {

public:
    //! Construct an empty library.
    //! \param detector_coords if true, store profiles in detector coordinates;
    //! otherwise store in Kabsch coordinates
    ShapeLibrary(bool detector_coords, double peakScale, double bkgBegin, double bkgEnd);

    //! Returns whether the library is stored in detector coords or Kabsch coords
    bool detectorCoords() const;

    //! Add a reference peak to the library
    bool addPeak(sptrPeak3D peak, Profile3D&& profile, Profile1D&& integrated_profile);

    //! Update the fitted covariances
    void updateFit(int num_iterations);

    //! Predict the (detector space) covariance of a given peak
    Eigen::Matrix3d predictCovariance(sptrPeak3D peak) const;

    //! Returns mean Pearson coefficient to measure quality of fit
    double meanPearson() const;

    //! Returns the average peak profile near the given detector event
    Profile3D meanProfile(const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the average peak profile near the given detector event
    std::vector<Intensity>
    meanProfile1D(const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the average peak covariance near the given detector event
    Eigen::Matrix3d meanCovariance(
        sptrPeak3D reference_peak, double radius, double nframes, size_t min_neighbors,
        PeakInterpolation interpolation) const;

    //! Find neighbors of a given peak
    PeakList findNeighbors(const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the peak scale used for the library
    double peakScale() const;

    //! Returns the background begin used for the library
    double bkgBegin() const;

    //! Returns the background end used for the library
    double bkgEnd() const;

private:
    //! Predict the (detector space) covariance given the fit data
    Eigen::Matrix3d predictCovariance(const FitData& data) const;

    //! List of reference peak profiles
    std::map<sptrPeak3D, std::pair<Profile3D, Profile1D>> _profiles;

    //! Components of the Cholesky factor of beam divergence covariance matrix
    std::array<double, 6> _choleskyD;

    //! Components of the Cholesky factor of mosaicity covariance matrix
    std::array<double, 6> _choleskyM;

    //! Components of the Cholesky factor of shape covariance matrix
    std::array<double, 6> _choleskyS;

    //! Sets true if the profiles are stored in detector space coordinates or false
    //! for Kabsch coords
    bool _detectorCoords;

    //! The peak scale used by the library for integration
    double _peakScale;

    //! The background begin used by the library for integration
    double _bkgBegin;

    //! The background end used by the library for integration
    double _bkgEnd;
};

} // end namespace nsx

#endif // CORE_INTEGRATION_SHAPELIBRARY_H
