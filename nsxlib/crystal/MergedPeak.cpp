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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <cmath>
#include <stdexcept>

#include "MergedPeak.h"


namespace nsx {

MergedPeak::MergedPeak(const SpaceGroup& grp, bool friedel):
    _hkl(), _intensity(0.0, 0.0), _chiSquared(0.0), _std(0.0), _peaks(), _grp(grp), _friedel(friedel), _d(0.0)
{
}


bool MergedPeak::addPeak(const sptrPeak3D& peak)
{
    auto hkl1 = _hkl.cast<double>();
    auto hkl2 = peak->getIntegerMillerIndices().cast<double>();
    // peak is not equivalent to one already on the list
    if (!_peaks.empty() && !_grp.isEquivalent(hkl1, hkl2, _friedel)) {
        return false;
    }

    // add peak to list
    _peaks.emplace_back(peak);

    // this was the first peak, so we have to update _hkl
    _hkl = peak->getIntegerMillerIndices();
    update();

    return true;
}

Eigen::Vector3i MergedPeak::getIndex() const
{
    return _hkl;
}

const Intensity &MergedPeak::getIntensity() const
{
    return _intensity;
}

double MergedPeak::chiSquared() const
{
    return _chiSquared;
}

size_t MergedPeak::redundancy() const
{
    //assert(_peaks.size() <= (_friedel ? 2:1) * _grp.getGroupElements().size());
    return _peaks.size();
}

double MergedPeak::std() const
{
    return _std;
}

void MergedPeak::determineRepresentativeHKL()
{
    Eigen::Vector3d best_hkl = _hkl.cast<double>();

    std::vector<Eigen::Vector3d> equivs;

    for (auto&& g: _grp.getGroupElements()) {
        equivs.emplace_back(g.getRotationPart()*best_hkl);
    }

    auto num_nneg = [](const Eigen::Vector3d& v) -> int {
        int neg = 0;

        for (int i = 0; i < 3; ++i) {
            if (v(i) >= 0) {
                ++neg;
            }
        }
        return neg;
    };

    auto compare_fn = [=](const Eigen::Vector3d& a, const Eigen::Vector3d& b) -> bool {
        const double eps = 1e-5;
        auto neg_a = num_nneg(a);
        auto neg_b = num_nneg(b);

        if (neg_a != neg_b) {
            return neg_a > neg_b;
        }

        if (std::abs(a(0)-b(0)) > eps) {
            return a(0) > b(0);
        }
        if (std::abs(a(1)-b(1)) > eps) {
            return a(1) > b(1);
        }
        return a(2) > b(2);
    };

    best_hkl = *std::min_element(equivs.begin(), equivs.end(), compare_fn);

    for (int i = 0; i < 3; ++i) {
        _hkl(i) = int(std::lround(best_hkl(i)));
    }
}

void MergedPeak::update()
{
    determineRepresentativeHKL();

    // update average intensity and error
    _intensity = Intensity(0.0, 0.0);

    double variance = 0.0;

    for (auto&& peak: _peaks) {
        _intensity += peak->getCorrectedIntensity();
    }

    _intensity = _intensity / _peaks.size();

    // update chi2
    // TODO(jonathan): check that this is correct!
    _chiSquared = 0.0;
    _std = 0.0;
    _d = 0.0;

    for (auto&& peak: _peaks) {
        const double difference = peak->getCorrectedIntensity().getValue() - _intensity.getValue();
        const double res2 = std::pow(difference, 2);
        _chiSquared += res2 / _intensity.getValue();
        _std += res2;
        _d += 1.0 / peak->getQ().norm();
    }

    _d /= _peaks.size();

    if (_peaks.size()>1) {
        _std /= (_peaks.size()-1);
    }
    _std = std::sqrt(_std);
}

double MergedPeak::d() const
{
    return _d;
}

const std::vector<sptrPeak3D> &MergedPeak::getPeaks() const
{
    return _peaks;
}

} // end namespace nsx
