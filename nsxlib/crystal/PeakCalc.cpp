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

#include <set>

#include <Eigen/Eigenvalues>

#include "../crystal/Peak3D.h"
#include "../crystal/PeakCalc.h"
#include "../data/DataSet.h"
#include "../geometry/Ellipsoid.h"
#include "../geometry/Octree.h"
#include "../instrument/ComponentState.h"
#include "../instrument/DetectorEvent.h"

namespace nsx {

PeakCalc::PeakCalc(int h, int k, int l, double x,double y, double frame):
    _h(h),_k(k),_l(l),_x(x),_y(y),_frame(frame), _intensity()
{
}

PeakCalc::PeakCalc(): _h(0), _k(0), _l(0), _x(0), _y(0), _frame(0), _intensity()
{

}

PeakCalc::PeakCalc(const Peak3D& peak): _intensity()
{
    auto hkl = peak.getIntegerMillerIndices();
    auto center = peak.getShape().aabb().center();

    _h = hkl(0);
    _k = hkl(1);
    _l = hkl(2);

    _x = center(0);
    _y = center(1);
    _frame = center(2);

    _intensity = peak.getCorrectedIntensity();
}


} // end namespace nsx
