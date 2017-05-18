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

#ifndef NSXLIB_INTERPOLATOR_H
#define NSXLIB_INTERPOLATOR_H

#include <vector>
#include "Round.h"

namespace nsx {

template<typename T_>
T_ interpolate(const std::vector<T_>& elements, double index) {
    if (index < 0)
        index = 0;

    if (index > elements.size()-1)
        index = elements.size()-1;

    auto index0 = ifloor(index);
    auto index1 = ifloor(index+1);

    if (index1 > elements.size()-1)
        index1 = elements.size()-1;

    double t = index-index0;


    assert(index0 >= 0);
    assert(index1 >= 0);

    assert(index0 < elements.size());
    assert(index1 < elements.size());

    assert(t >= 0.0);
    assert(t <= 1.0);


    return (1-t)*elements[index0] + t*elements[index1];
}

} // end namespace nsx

#endif // NSXLIB_INTERPOLATOR_H
