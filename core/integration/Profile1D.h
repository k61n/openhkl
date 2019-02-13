#pragma once

#include <vector>

#include <Eigen/Dense>

#include "Intensity.h"

namespace nsx {

//! Class to produce a histogram of 1d integrated intensity profiles,
//! For use in the sigma/I integration method.
class Profile1D {
public:
    //! Constructor. sigma_max indicates maximum number of standard deviations
    Profile1D(const Intensity& mean_background = {}, double sigma_max = 4.0, size_t num=200);
    //! Add a data point to the bins.
    //! Parameter r2 should be equal to (x-x0).dot(A*(x-x0)) where A is the inverse covariance matrix
    //! of the peak.
    //! Parameter M is the total count (no background correction)
    void addPoint(double r2, double M);
    //! Return the vector of integrated counts values
    const std::vector<double>& counts() const;
    //! Return the number of points in each bin
    const std::vector<int>& npoints() const;
    //! Return the profile I(s) / I(smax)
    std::vector<Intensity> profile() const;
    //! Reset the profile to zero
    void reset();

private:
    std::vector<double> _counts;
    std::vector<int> _npoints;
    std::vector<double> _endpoints;
    double _sigmaMax;
    Intensity _meanBkg;
};

} // end namespace nsx
