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
#ifndef NSXTOOL_BBOBJECT_H_
#define NSXTOOL_BBOBJECT_H_

#include <vector>

#include "NDVector.h"

namespace SX
{

namespace Geometry
{

template <typename T, size_t D> class NDAABBObject
{
public:
	//! Constructors and operators
	NDAABBObject();

	NDAABBObject(const NDVector<T,D>& lBound, const NDVector<T,D>& uBound);

	NDAABBObject(const NDAABBObject<T,D>& other);

	NDAABBObject& operator=(const NDAABBObject<T,D>& other);

	//! lower bound constant getter
	const NDVector<T,D>& getLowerBound() const;

	//! lower bound getter
	NDVector<T,D>& getLowerBound();

	//! upper bound constant getter
	const NDVector<T,D>& getUpperBound() const;

	//! upper bound getter
	NDVector<T,D>& getUpperBound();

	//! return the center of the AABB
	NDVector<T,D> getCenter() const;

	//! Return the extents of the AABB
	std::vector<NDVector<T,D>> getExtents() const;

	//! Inline really makes a difference in speed here.
	inline bool intercept(const NDAABBObject<T,D>& other) const
	{
		bool cond(true);
		for (size_t i=0; i<D; ++i)
		{
			cond &=(_upperBound[i] > other._lowerBound[i]);
			cond &=(_lowerBound[i] < other._upperBound[i]);
		}
		return cond;
	}


protected:
	// The lower bound point
	NDVector<T,D> _lowerBound;
	// The upper bound point
	NDVector<T,D> _upperBound;

};

template<typename T, size_t D>
NDAABBObject::NDAABBObject(): _lowerBound(0), _upperBound(1)
{
}

AABBObject::AABBObject(const V3D& lBound, const V3D& uBound): _lowerBound(lBound), _upperBound(uBound)
{
}

AABBObject::AABBObject(const AABBObject& other)
{
	_lowerBound = other._lowerBound;
	_upperBound = other._upperBound;
}

AABBObject& AABBObject::operator=(const AABBObject& other)
{
  if (this != &other)
  {
	  _lowerBound = other._lowerBound;
	  _upperBound = other._upperBound;
  }
  return *this;
}

const V3D& AABBObject::getLowerBound() const
{
	return _lowerBound;
}

V3D& AABBObject::getLowerBound()
{
	return _lowerBound;
}

const V3D& AABBObject::getUpperBound() const
{
	return _upperBound;
}

V3D& AABBObject::getUpperBound()
{
	return _upperBound;
}

V3D AABBObject::getCenter() const
{
	return (_lowerBound + _upperBound)/2.0;
}

std::vector<V3D> AABBObject::getExtents() const
{
	std::vector<V3D> extents;
	extents.reserve(2);

	V3D center(getCenter());

	extents.push_back(_upperBound-center);
	extents.push_back(_lowerBound-center);

	return extents;
}





} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_BBOBJECT_H_*/
