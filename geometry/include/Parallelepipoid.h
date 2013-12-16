/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
	BP 156
	6, rue Jules Horowitz
	38042 Grenoble Cedex 9
	France
	chapon[at]ill.fr
    pellegrini[at]ill.fr

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

#ifndef NSXTOOL_PARRALLELEPIPOID_H_
#define NSXTOOL_PARRALLELEPIPOID_H_

#include <initializer_list>

#include "IPShape.h"

namespace SX
{

namespace Geometry
{

template<typename T>
class Parallelepipoid : public IPShape<T,3>
{
public:
	Parallelepipoid();

	Parallelepipoid(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);

	~Parallelepipoid();

	bool is_inside(const std::initializer_list<T>& point) const;

};

template<typename T>
Parallelepipoid<T>::Parallelepipoid() : IPShape<T,3>()
{
}

template<typename T>
Parallelepipoid<T>::Parallelepipoid(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub) : IPShape<T,3>(lb,ub)
{
}

template<typename T>
Parallelepipoid<T>::~Parallelepipoid()
{
}

template<typename T>
bool Parallelepipoid<T>::is_inside(const std::initializer_list<T>& point) const
{
	return this->is_inside_aabb(point);
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_PARRALLELEPIPOID_H_
