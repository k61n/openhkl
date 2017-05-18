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

#ifndef NSXLIB_PEAKRECORD_H
#define NSXLIB_PEAKRECORD_H

namespace nsx {

class Peak3D;
class MergedPeak;

class PeakRecord {
public:
    int h, k, l;
    double x, y, z;
    double iobs, sigma;
    bool merged;

    // conversion operators
    PeakRecord(const Peak3D& other);
    PeakRecord(const MergedPeak& other);

    bool operator<(const PeakRecord& other) const;
};

} // end namespace nsx

#endif // NSXLIB_PEAKRECORD_H
