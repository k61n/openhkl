#pragma once

#include <vector>

#include <Eigen/Dense>

#include "AABB.h"
#include "Intensity.h"

namespace nsx {

//! \class IntegratedProfile
//! Class to produce a histogram of 1d integrated intensity profiles,
//! For use in the sigma/I integration method.
class IntegratedProfile {
public:
    //! Constructor. sigma_max indicates maximum number of standard deviations
    IntegratedProfile(const Intensity& mean_background = {}, double sigma_max = 4.0, size_t num=20);

    //! Add a data point to the bins.
    //! Parameter r2 should be equal to (x-x0).dot(A*(x-x0)) where A is the inverse covariance matrix
    //! of the peak.
    void add(double r2, const Intensity& I);

    //! Add another profile to this one. Must have same sigma_max and number of bins.
    //! Throws if these conditions are violated.
    void add(const IntegratedProfile& other);

    //! Return the vector of bin values
    const std::vector<Intensity>& bins() const;

    //! Return the number of points in each bin
    const std::vector<int>& npoints() const;

    //! Return the index of the bin with smallest sigma/I
    size_t optimalBin() const;

    //! Rescale the profile: I -> s*I
    void rescale(double s);

    //! Divide the profile by a given intensity, with propagation of errors
    void divide(const Intensity& I);

private:
    std::vector<Intensity> _profile;
    std::vector<int> _npoints;
    std::vector<double> _endpoints;
    double _sigmaMax;
    Intensity _meanBkg;
};

} // end namespace nsx
