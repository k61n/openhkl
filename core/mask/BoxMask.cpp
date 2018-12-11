/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
    2016- Laurent C. Chapon Eric Pellegrini, Jonathan Fisher
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


#include "BoxMask.h"
#include "Ellipsoid.h"

namespace nsx {

BoxMask::BoxMask(const AABB& aabb) : IMask(), _aabb(aabb) {}

bool BoxMask::collide(const Ellipsoid& ellipsoid) const
{
    return _aabb.collide(ellipsoid);
}

IMask* BoxMask::clone() const
{
    return new BoxMask(*this);
}

} // end namespace nsx
