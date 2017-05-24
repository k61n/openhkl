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



#include "CC.h"

namespace nsx {

CC::CC():_CChalf(0), _CCstar(0)
{

}

void CC::calculate(const std::vector<MergedPeak>& peaks)
{
    double xx, xy, yy, x, y;
    xx = xy = yy = x = y = 0.0;
    _nPeaks = 0;

    for (auto&& peak: peaks) {
        if (peak.redundancy() < 2) {
            continue;
        }

        auto split = peak.split();
        MergedPeak& p1 = split.first;
        MergedPeak& p2 = split.second;

        const double I1 = p1.getIntensity().getValue();
        const double I2 = p2.getIntensity().getValue();

        xx += I1*I1;
        xy += I1*I2;
        yy += I2*I2;
        x += I1;
        y += I2;

        ++_nPeaks;
    }

    const double numerator = xy - x*y / _nPeaks;
    const double varx = xx - x*x / _nPeaks;
    const double vary = yy - y*y / _nPeaks;

    _CChalf = numerator / std::sqrt(varx*vary);
    _CCstar = std::sqrt( 2*_CChalf / (1.0 + _CChalf));
}

double CC::CChalf() const
{
    return _CChalf;
}

double CC::CCstar() const
{
    return _CCstar;
}

unsigned int CC::nPeaks() const
{
    return _nPeaks;
}

} // end namespace nsx
