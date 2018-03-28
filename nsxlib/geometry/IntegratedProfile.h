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
    IntegratedProfile(double sigma_max = 4.0, size_t num=10);

    //! Add a data point to the bins.
    //! Parameter r2 should be equal to (x-x0).dot(A*(x-x0)) where A is the inverse covariance matrix
    //! of the peak.
    void add(double r2, const Intensity& I);

    //! Return the vector of bins
    const std::vector<Intensity> bins() const;

    //! Return the index of the bin with smallest sigma/I
    size_t optimalBin() const;

private:
    std::vector<Intensity> _profile;
    std::vector<double> _endpoints;
    double _dr3;
};

} // end namespace nsx
