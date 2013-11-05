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

class BBObject
{
public:
	// Constructors and operators
	BBObject();

	BBObject(const V3D& lBound, const V3D& uBound);

	BBObject(const BBObject& other);

	BBObject& operator=(const BBObject& other);

	// lower bound constant getter
	const V3D& getLowerBound() const;

	// lower bound getter
	V3D& getLowerBound() const;

	// upper bound constant getter
	const V3D& getUpperBound() const;

	// upper bound getter
	V3D& getUpperBound() const;

	// return the center of the BB
	V3D getCenter() const;

	// Return the extents of the BB
	std::vector<V3D> getExtents() const;

	void intercept(const BBObject& other) = 0;


private:
	// The lower bound point
	V3D _lowerBound;
	// The upper bound point
	V3D _upperBound;

};

} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_BBOBJECT_H_*/
