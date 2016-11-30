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

 Forshungszentrum Juelich GmbH
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


namespace SX
{
namespace Crystal
{

MergedPeak::MergedPeak(SpaceGroup grp):
    _grp{grp}, _peaks{}, _intensity{0}, _sigma{0}, _hkl(), _chiSquared(0.0)
{

}

bool MergedPeak::addPeak(sptrPeak3D peak)
{
    // peak is not equivalent to one already on the list
    if (_peaks.size() && !_grp.isEquivalent(_hkl.cast<double>(), peak->getMillerIndices()))
        return false;

    // add peak to list
    _peaks.push_back(peak);

    // if this was NOT the first list, nothing remains to be done
    if ( _peaks.size() >= 2)
        return true;

    // this was the first peak, so we have to update _hkl
    _hkl = peak->getIntegerMillerIndices();
    update();
}

Eigen::Vector3i MergedPeak::getIndex() const
{
    return _hkl;
}

double MergedPeak::intensity() const
{
    return _intensity;
}

double MergedPeak::sigma() const
{
    return _sigma;
}

double MergedPeak::chiSquared() const
{
    return _chiSquared;
}

void MergedPeak::determineRepresentativeHKL()
{
    Eigen::Vector3d best_hkl = _hkl.cast<double>();

    std::vector<Eigen::Vector3d> equivs;

    for (auto&& g: _grp.getGroupElements())
        equivs.push_back(g.getRotationPart()*best_hkl);

    auto compare_fn = [](const Eigen::Vector3d& a, const Eigen::Vector3d& b) -> bool
    {
        if (a(0) != b(0))
            return a(0) > b(0);
        else if (a(1) != b(1))
            return a(1) > b(1);
        else
            return a(2) > b(2);
    };

    best_hkl = *std::min_element(equivs.begin(), equivs.end(), compare_fn);
}

void MergedPeak::update()
{
    determineRepresentativeHKL();

    // update average intensity and error
    _intensity = 0.0;
    _sigma = 0.0;
    double variance = 0.0;

    for (auto&& peak: _peaks) {
        _intensity += peak->getScaledIntensity();
        variance += std::pow(peak->getScaledSigma(), 2);
    }

    _intensity /= _peaks.size();
    variance /= _peaks.size();
    _sigma = std::sqrt(variance);

    // update chi2
    // TODO: check that this is correct!
    _chiSquared = 0.0;

    for (auto&& peak: _peaks)
        _chiSquared += std::pow((peak->getScaledIntensity() - _intensity), 2) / variance;
}



}
}




