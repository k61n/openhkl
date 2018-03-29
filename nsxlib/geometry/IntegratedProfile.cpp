#include "IntegratedProfile.h"

#include <cmath>

namespace nsx {

IntegratedProfile::IntegratedProfile(double sigma_max, size_t num): _profile(num, 0.0), _endpoints(num+1), _sigmaMax(sigma_max)
{
    const double dr3 = sigma_max*sigma_max*sigma_max / num;

    for (size_t i = 0; i < num+1; ++i) {
        _endpoints[i] = std::pow(i*dr3, 2.0/3.0);
    }
}

void IntegratedProfile::add(double r2, const Intensity& I)
{
    // outside of allowable range
    if (r2 < 0 || r2 > _endpoints.back()) {
        return;
    }

    for (size_t i = 0; i < _profile.size(); ++i) {
        if (r2 <= _endpoints[i+1]) {
            _profile[i] += I;
        }
    }
}

const std::vector<Intensity> IntegratedProfile::bins() const
{
    return _profile;
}

size_t IntegratedProfile::optimalBin() const
{
    double best_Isigma = _profile[0].value() / _profile[0].sigma();
    size_t best_idx = 0;

    for (size_t i = 0; i < _profile.size(); ++i) {
        double Isigma = _profile[i].value() / _profile[i].sigma();
        if (!(Isigma <= best_Isigma)) {
            best_Isigma = i;
            best_idx = i;
        }
    }
    return best_idx;
}

void IntegratedProfile::add(const IntegratedProfile& other)
{
    if (_profile.size() != other._profile.size()) {
        throw std::invalid_argument("Cannot add two profiles with different number of bins");
    }

    if (std::fabs(_sigmaMax - other._sigmaMax) > 1e-6) {
        throw std::invalid_argument("Cannot add two profiles with different _sigmaMax");
    }

    for (size_t i = 0; i < _profile.size(); ++i) {
        _profile[i] += other._profile[i];
    }
}

void IntegratedProfile::rescale(double s) {
    for (auto& I: _profile) {
        I *= s;
    }
}

void IntegratedProfile::divide(const Intensity& I)
{
    for (auto& I0: _profile) {
        I0 = I0 / I;
    }
}

} // end namespace nsx
