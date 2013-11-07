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

#include "V3D.h"

namespace SX
{

namespace Geometry
{

class AABBObject
{
public:
	//! Constructors and operators
	AABBObject();

	AABBObject(const V3D& lBound, const V3D& uBound);

	AABBObject(const AABBObject& other);

	AABBObject& operator=(const AABBObject& other);

	//! lower bound constant getter
	const V3D& getLowerBound() const;

	//! lower bound getter
	V3D& getLowerBound();

	//! upper bound constant getter
	const V3D& getUpperBound() const;

	//! upper bound getter
	V3D& getUpperBound();

	//! return the center of the BB
	V3D getCenter() const;

	//! Return the extents of the BB
	std::vector<V3D> getExtents() const;

	//! Inline really makes a difference in speed here.
	inline bool intercept(const AABBObject& other) const
	{
		return(_upperBound[0] > other._lowerBound[0] &&
		    _lowerBound[0] < other._upperBound[0] &&
		    _upperBound[1] > other._lowerBound[1] &&
		    _lowerBound[1] < other._upperBound[1] &&
		    _upperBound[2] > other._lowerBound[2] &&
		    _lowerBound[2] < other._upperBound[2]);
	}


protected:
	// The lower bound point
	V3D _lowerBound;
	// The upper bound point
	V3D _upperBound;

};


} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_BBOBJECT_H_*/
