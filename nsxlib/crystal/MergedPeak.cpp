/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNUctest
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>

#include "MergedPeak.h"
#include "Peak3D.h"
#include "../statistics/ChiSquared.h"

namespace nsx {

MergedPeak::MergedPeak(const SpaceGroup& grp, bool friedel):
    _hkl(), _intensitySum(0.0, 0.0), _peaks(), _grp(grp), _friedel(friedel), _squaredIntensitySum(0.0)
{
}

bool MergedPeak::addPeak(const sptrPeak3D& peak)
{
    return addPeak(PeakCalc(*peak));
}

bool MergedPeak::addPeak(const PeakCalc& peak)
{
    auto hkl1 = _hkl.cast<double>();
    auto hkl2 = Eigen::Vector3d(peak._h, peak._k, peak._l);
    // peak is not equivalent to one already on the list
    if (!_peaks.empty() && !_grp.isEquivalent(hkl1, hkl2, _friedel)) {
        return false;
    }

    // add peak to list
    _peaks.emplace_back(peak);

    // if this was the first peak, we have to update _hkl
    if (_peaks.size() == 1) {
        for (auto i = 0; i < 3; ++i) {
            _hkl(i) = hkl2(i);
        }
        determineRepresentativeHKL();
    }

    _intensitySum += peak._intensity;
    _squaredIntensitySum += std::pow(peak._intensity.getValue(), 2);

    return true;
}

Eigen::RowVector3i MergedPeak::getIndex() const
{
    return _hkl;
}

Intensity MergedPeak::getIntensity() const
{
    return _intensitySum / _peaks.size();
}

size_t MergedPeak::redundancy() const
{
    return _peaks.size();
}

#if 0
double MergedPeak::std() const
{
    const double n = _peaks.size();
    const double I = getIntensity().getValue() / _peaks.size();
    const double var = (_squaredIntensitySum - n*I*I) / (n-1);
    return std::sqrt(var);
}
#endif 

void MergedPeak::determineRepresentativeHKL()
{
    Eigen::Vector3d best_hkl = _hkl.cast<double>();
    std::vector<Eigen::Vector3d> equivs;

    for (auto&& g: _grp.groupElements()) {
        equivs.emplace_back(g.getRotationPart()*best_hkl);

        if (_friedel) {
            equivs.emplace_back(-g.getRotationPart()*best_hkl);
        }
    }

    auto compare_fn = [=](const Eigen::Vector3d& a, const Eigen::Vector3d& b) -> bool {
        const double eps = 1e-5;
        for (auto i = 0; i < 3; ++i) {
            if (std::abs(a(i)-b(i)) > eps) {
                return a(i) > b(i);
            }
        }
        return false;
    };

    best_hkl = *std::min_element(equivs.begin(), equivs.end(), compare_fn);

    for (int i = 0; i < 3; ++i) {
        _hkl(i) = int(std::lround(best_hkl(i)));
    }
}

const std::vector<PeakCalc>& MergedPeak::getPeaks() const
{
    return _peaks;
}

std::pair<MergedPeak, MergedPeak> MergedPeak::split() const
{
    // make copy of peak list
    std::vector<size_t> random_idx(_peaks.size());

    for (unsigned int j = 0; j < _peaks.size(); ++j) {
        random_idx[j] = j;
    }

    // randomly reorder
    std::random_shuffle(random_idx.begin(), random_idx.end());

    unsigned int parity = std::rand()%2;

    MergedPeak p1(_grp, _friedel), p2(_grp, _friedel);

    for (unsigned int i = 0; i < _peaks.size(); ++i) {
        auto idx = random_idx[i];
        auto p = _peaks[idx];

        if ((i%2) == parity) {
            p1.addPeak(p);
        }
        else {
            p2.addPeak(p);
        }
    }

    return std::make_pair(p1, p2);
}


bool operator<(const MergedPeak& p, const MergedPeak& q)
{
    const auto& a = p.getIndex();
    const auto& b = q.getIndex();

    if (a(0) != b(0)) {
        return a(0) < b(0);
    }
    if (a(1) != b(1)) {
        return a(1) < b(1);
    }
    return a(2) < b(2);
}

//! The merged intensity and error are computed as \f$I_{\mathrm{merge}} = N^{-1} \sum_i I_i\f$, 
//! \f$\sigma_{\mathrm{merge}}^2 = N^{-2}\sum_i \sigma_i^2\f$.
//! where \f$N\f$ is the redundancy of the reflection. This method computes the statistic
//! \f[ \chi^2 = \frac{\sum_i (I_i-I_{\mathrm{merge}})^2}{N \sigma_{\mathrm{merge}}^2}, \f]
//! which is approximately a chi-squared statistic with \f$N-1\f$ degrees of freedom.
double MergedPeak::chi2() const
{
    const double I_merge = getIntensity().getValue();
    const double sigma_merge = getIntensity().getSigma();
    const double N = redundancy();

    // if there is no redundancy, we cannot compute chi2
    if (N < 1.99) {
        return 0.0;
    }

    double chi_sq = 0.0;

    for (auto&& peak: _peaks) {
        auto&& I = peak._intensity; 
        const double x = (I.getValue() - I_merge) / sigma_merge;
        chi_sq += x*x/N;
    }
    return chi_sq;
}

//! This method returns the probability that a chi-squared random variable takes a value less than
//! the computed value of MergedPeak::chi2(). A large p-value indicates that the computed variance
//! is larger than the expected variance, indicating the possibility of a systematic error either 
//! in the integrated intensities or in the computed error. 
double MergedPeak::pValue() const
{
    // todo: k or k-1?? need to check
    const double k = redundancy()-1.0;

    // if there is only one observation, we cannot compute a p-value
    if (k < 0.9) {
        return 0.0;
    }

    const double x = chi2();
    ChiSquared chi(k);
    return chi.cdf(x);
}

} // end namespace nsx
