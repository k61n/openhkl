/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include "PeakRecord.h"
#include "Peak3D.h"
#include "MergedPeak.h"

namespace nsx {

#if 0

PeakRecord::PeakRecord(const Peak3D &other)
{
    auto&& index = other.getIntegerMillerIndices();
    auto&& center = other.getShape().aabb().center();

    h = index[0];
    k = index[1];
    l = index[2];

    x = center[0];
    y = center[1];
    z = center[2];

    merged = false;
    auto intensity = other.getScaledIntensity();
    iobs = intensity.value();
    sigma = intensity.sigma();
}

PeakRecord::PeakRecord(const MergedPeak& other)
{
    auto&& index = other.getIndex();

    h = index[0];
    k = index[1];
    l = index[2];

    x = y = z = 0.0;
    merged = true;

    auto intensity = other.getIntensity();
    iobs = intensity.value();
    sigma = intensity.sigma();
}

bool PeakRecord::operator<(const PeakRecord &other) const
{
    if (h < other.h)
        return true;
    if (h > other.h)
        return false;
    if (k < other.k)
        return true;
    if (k > other.k)
        return false;
    if (l < other.l)
        return true;
    if (l > other.l)
        return false;

    if (x < other.x)
        return true;
    if (x > other.x)
        return false;

    if (y < other.y)
        return true;
    if (y > other.y)
        return false;
    if (z < other.z)
        return true;
    if (z > other.z)
        return false;

    return iobs < other.iobs;
}

#endif
} // end namespace nsx
