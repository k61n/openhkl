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

#ifndef NSXTOOL_SHAPECOLLECTION_H_
#define NSXTOOL_SHAPECOLLECTION_H_

#include <vector>

#include "AABB.h"
#include "IPShape.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

template<typename T,uint D>
class ShapeCollection : public AABB<T,D>
{
public:
	ShapeCollection();

	//! add a primitive shape
	void add_shape(const IPShape<T,D>& shape);

	//! return whether or not a given point is inside one of the primitive shape the collection
	virtual bool is_inside(const std::initializer_list<T>& point) const;

	//! return whether or not a given shape contains a given point
	virtual bool is_inside(const IPShape<T,D>& shape, const std::initializer_list<T>& point) const;

	//! return a vector of the primitive shapes that contains a given point
	std::vector<IPShape<T,D>> where_is(const std::initializer_list<T>& point) const;

	//! return the current number of shapes
	int n_shapes() const;

protected:
	//! store pointers to the primitive shapes
	std::vector<IPShape<T,D>> _shapes;
};

template<typename T, uint D>
ShapeCollection<T,D>::ShapeCollection() : AABB<T,D>()
{
	_shapes.reserve(3);
}

template<typename T,uint D>
void ShapeCollection<T,D>::add_shape(const IPShape<T,D>& shape)
{
	_shapes.push_back(shape);
}

template<typename T,uint D>
int ShapeCollection<T,D>::n_shapes() const
{
	return _shapes.size();
}

template<typename T, uint D>
bool ShapeCollection<T,D>::is_inside(const std::initializer_list<T>& point) const
{
	for (auto it=_shapes.begin(); it!=_shapes.end(); ++it)
	{
		if ((*it).is_inside(point))
			return true;
	}
	return false;
}

template<typename T, uint D>
bool ShapeCollection<T,D>::is_inside(const IPShape<T,D>& shape, const std::initializer_list<T>& point) const
{

	auto it=std::find(_shapes.begin(), _shapes.end(), shape);
	if (it==_shapes.end())
		throw std::invalid_argument("ShapeCollection: the input shape is not stored in the composite shape");

	return (*it).is_inside(point);

}

template<typename T, uint D>
std::vector<IPShape<T,D>> ShapeCollection<T,D>::where_is(const std::initializer_list<T>& point) const
{

	std::vector<IPShape<T,D>> sh;
	sh.reserve(_shapes.size());
	for (auto it=_shapes.begin(); it!=_shapes.end(); ++it)
	{
		if ((*it).is_inside(point))
			sh.push_back(*it);
	}
	return sh;
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_SHAPECOLLECTION_H_
