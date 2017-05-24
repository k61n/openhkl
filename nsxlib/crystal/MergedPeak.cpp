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

<<<<<<< HEAD
#include "../crystal/MergedPeak.h"
#include "../crystal/Peak3D.h"
=======
#include "MergedPeak.h"
#include "Peak3D.h"

>>>>>>> add CC factor calculation

namespace nsx {

MergedPeak::MergedPeak(const SpaceGroup& grp, bool friedel):
    _hkl(), _intensitySum(0.0, 0.0), _peaks(), _grp(grp), _friedel(friedel), _dSum(0.0), _squaredIntensitySum(0.0)
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

    // if this was the first peak, we have to update _hkl
    if (_peaks.size() == 1) {
        for (auto i = 0; i < 3; ++i) {
            _hkl(i) = hkl2(i);
        }
        determineRepresentativeHKL();
    }

    _intensitySum += peak->getCorrectedIntensity();
    _squaredIntensitySum += std::pow(peak->getCorrectedIntensity().getValue(), 2);
    _dSum += 1.0 / peak->getQ().norm();

    return true;
}

Eigen::Vector3i MergedPeak::getIndex() const
{
    return _hkl;
}

Intensity MergedPeak::getIntensity() const
{
    return _intensitySum / _peaks.size();
}

size_t MergedPeak::redundancy() const
{
    //assert(_peaks.size() <= (_friedel ? 2:1) * _grp.getGroupElements().size());
    return _peaks.size();
}

double MergedPeak::std() const
{
    const double n = _peaks.size();
    const double I = getIntensity().getValue() / _peaks.size();
    const double var = (_squaredIntensitySum - n*I*I) / (n-1);
    return std::sqrt(var);
}

void MergedPeak::determineRepresentativeHKL()
{
    Eigen::Vector3d best_hkl = _hkl.cast<double>();
    std::vector<Eigen::Vector3d> equivs;

    for (auto&& g: _grp.getGroupElements()) {
        equivs.emplace_back(g.getRotationPart()*best_hkl);
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

double MergedPeak::d() const
{
    return _dSum / _peaks.size();
}

const PeakList& MergedPeak::getPeaks() const
{
    return _peaks;
}

std::pair<MergedPeak, MergedPeak> MergedPeak::split() const
{
    // make copy of peak list
    std::vector<sptrPeak3D> random_peaks = _peaks;
    // randomly reorder
    std::random_shuffle(random_peaks.begin(), random_peaks.end());

    unsigned int i = 0;
    unsigned int parity = std::rand()%2;

    MergedPeak p1(_grp, _friedel), p2(_grp, _friedel);

    for (auto&& p: random_peaks) {
        if ((i%2) == parity) {
            p1.addPeak(p);
        }
        else {
            p2.addPeak(p);
        }
        ++i;
    }

    return std::make_pair(p1, p2);
}

} // end namespace nsx
