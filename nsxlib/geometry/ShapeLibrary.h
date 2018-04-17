#pragma once

#include <Eigen/Dense>
#include <array>
#include <map>
#include <vector>

#include "CrystalTypes.h"
#include "DetectorEvent.h"
#include "FitProfile.h"
#include "IntegratedProfile.h"
#include "MillerIndex.h"

namespace nsx {

struct FitData;

//! \brief Class to store a library of peak shapes _as covariance matrices n q space_.
//!
class ShapeLibrary {
public:
    ShapeLibrary();
    ~ShapeLibrary();

    //! Add a refernce peak to the library
    bool addPeak(sptrPeak3D peak, FitProfile&& profile, IntegratedProfile&& integrated_profile);

    //! Update the fitted covariances
    void updateFit(int num_iterations);

    //! Predict the (detector space) covariance of a given peak
    Eigen::Matrix3d predictCovariance(sptrPeak3D peak) const;

    //! Predict the (detector space) covariance given the fit data
    Eigen::Matrix3d predictCovariance(const FitData& data) const;

    //! Return mean Pearson coefficient to measure quality of fit
    double meanPearson() const;

    //! Return the average peak profile near the given detector event
    FitProfile meanProfile(const DetectorEvent& ev, double radius, double nframes) const;

    //! Return the average peak profile near the given detector event
    std::vector<Intensity> meanIntegratedProfile(const DetectorEvent& ev, double radius, double nframes) const;

    //! Return the average peak covariance near the given detector event
    Eigen::Matrix3d meanCovariance(sptrPeak3D reference_peak, double radius, double nframes, size_t min_neighbors) const;

    //! Find neighbors of a given peak
    PeakList findNeighbors(const DetectorEvent& ev, double radius, double nframes) const;

private:
    std::map<sptrPeak3D, std::pair<FitProfile, IntegratedProfile>> _profiles;

    //! Components of the Cholesky factor of beam divergence covariance matrix
    std::array<double, 6> _choleskyD;

    //! Components of the Cholesky factor of mosaicity covariance matrix
    std::array<double, 6> _choleskyM;

    //! Components of the Cholesky factor of shape covariance matrix
    std::array<double, 6> _choleskyS;
};

} // end namespace nsx
