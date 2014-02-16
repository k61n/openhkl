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

#ifndef NSXTOOL_NDBOX_H_
#define NSXTOOL_NDBOX_H_

#include <initializer_list>

#include "IPShape.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

template<typename T,uint D>
class NDBox : public IShape<T,D>
{
public:
	NDBox();

	NDBox(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);

	~NDBox();

	bool collide(const IShape<T,D>& rhs) const;
	bool collide(const NDBox<T,D>& rhs) const;

	bool is_inside(const std::initializer_list<T>& point) const;

};

template<typename T,uint D>
NDBox<T,D>::NDBox() : IShape<T,D>()
{
}

template<typename T,uint D>
NDBox<T,D>::NDBox(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub) : IShape<T,D>(lb,ub)
{
}

template<typename T,uint D>
NDBox<T,D>::~NDBox()
{
}

template<typename T,uint D>
bool collide(const IShape<T,D>& rhs) const
{
    rhs.collide(*this);
}

template<typename T,uint D>
bool collide(const NDBox<T,D>& rhs) const
{
    return collisionBoxBox<T,D>(*this,rhs);
}


template<typename T,uint D>
bool NDBox<T,D>::is_inside(const std::initializer_list<T>& point) const
{
	return this->is_inside_aabb(point);
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_NDBOX_H_
