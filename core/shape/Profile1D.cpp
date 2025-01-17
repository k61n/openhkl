//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/Profile1D.cpp
//! @brief     Implements class Profile1D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/Profile1D.h"

#include <cmath>
#include <cstddef>

namespace ohkl {

Profile1D::Profile1D(const Intensity& mean_background, double sigma_max, size_t num)
    : _counts(num, 0.0)
    , _npoints(num, 0)
    , _endpoints(num + 1)
    , _meanBkg(mean_background)
    , _mean_profile()
    , _nprofiles(0)
{
    const double dr3 = sigma_max * sigma_max * sigma_max / num;

    for (size_t i = 0; i < num + 1; ++i)
        _endpoints[i] = std::pow(i * dr3, 2.0 / 3.0);
}

void Profile1D::addPoint(double r2, double M)
{
    // outside of allowable range
    if (r2 < 0 || r2 > _endpoints.back())
        return;

    for (size_t i = 0; i < _counts.size(); ++i) {
        if (r2 <= _endpoints[i + 1]) {
            _counts[i] += M;
            _npoints[i] += 1;
        }
    }
}

void Profile1D::addProfile(const Profile1D& profile)
{
    if (_mean_profile.empty())
        _mean_profile.resize(profile.profile().size());

    std::vector<Intensity> rocking_curve = profile.profile();
    for (std::size_t idx = 0; idx < rocking_curve.size(); ++idx)
        _mean_profile[idx] += rocking_curve[idx];
    ++_nprofiles;
}

void Profile1D::normalize()
{
    const double inv_N = 1.0 / _nprofiles;
    for (std::size_t i = 0; i < _mean_profile.size(); ++i)
        _mean_profile[i] *= inv_N;
}

const std::vector<double>& Profile1D::counts() const
{
    return _counts;
}

const std::vector<int>& Profile1D::npoints() const
{
    return _npoints;
}

std::vector<Intensity> Profile1D::profile() const
{
    std::vector<Intensity> profile(_counts.size(), 0.0);
    const double mean_bkg = _meanBkg.value();
    const double var_bkg = _meanBkg.variance();

    // maximum intensity
    const size_t nmax = _npoints.back();
    const double I_max = _counts.back() - nmax * mean_bkg;
    const double var_max = _counts.back() + nmax * nmax * var_bkg;
    const double I3 = I_max * I_max * I_max;
    const double I4 = I_max * I_max * I_max * I_max;

    for (size_t i = 0; i < _counts.size(); ++i) {
        const int n = _npoints[i];
        const double I = _counts[i] - n * mean_bkg;
        const double I_var = _counts[i] + n * n * var_bkg;

        // profile value
        const double p = I / I_max;

        // in what remaines, we compute profile variance.
        // Note there is covariance between I and I_max!!

        // variance due to I
        double p_var = I_var / I / I;

        // variance due to I_max
        p_var += I * I * var_max / I4;

        // covariance between I and I_max. Note the sign!
        const double cov = _counts[i] + n * nmax * var_bkg;
        p_var -= 2 * I / I3 * cov;

        // record result
        profile[i] = Intensity(p, p_var);
    }
    return profile;
}

void Profile1D::reset()
{
    std::fill(_counts.begin(), _counts.end(), 0);
    std::fill(_npoints.begin(), _npoints.end(), 0);
}

} // namespace ohkl
