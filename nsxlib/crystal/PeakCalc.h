/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr
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

#ifndef NSXLIB_PEAKCALC_H
#define NSXLIB_PEAKCALC_H

#include "../crystal/CrystalTypes.h"
#include "../geometry/GeometryTypes.h"
#include "../crystal/Intensity.h"

namespace nsx {

struct PeakCalc {

    PeakCalc(const Peak3D& peak);
    PeakCalc();
    PeakCalc(int h, int k, int l, double x,double y, double frame); 
    ~PeakCalc() = default;

    Intensity _intensity;
    int _h,_k,_l;
    double _x,_y,_frame;

    sptrPeak3D averagePeaks(const Octree& tree, double distance, double min_axis=2.0);
};

} // end namespace nsx

#endif // NSXLIB_PEAKCALC_H
