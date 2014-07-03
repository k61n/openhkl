/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
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

#ifndef NSXTOOL_ROTATION_H_
#define NSXTOOL_ROTATION_H_

#include <map>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include "PrimitiveTransformation.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

using boost::property_tree::ptree;

class Rotation : public PrimitiveTransformation
{
public:

	//! Enumerates the possible rotation directions (clockwise-->CW or counter clockwise-->CCW).
	enum class Direction {CW=0,CCW=1};

	static PrimitiveTransformation* Create(const ptree&);

	//! Default constructor.
	Rotation();
	//! Explicit constructor.
	Rotation(const Vector3d&, Direction, double);
	//! Constructor from an XML node.
	Rotation(const ptree&);
	//! The destructor.
	~Rotation();

	//! Returns the rotation part of the homogeneous matrix.
	Matrix3d getRotation(double) const;
	// ! Return 0 for CCW and 1 for CW.
	Direction getRotationDirection() const;
	//! Get rotation direction.
	void setRotationDirection(Direction);
	//! Returns the homogeneous matrix.
	HomMatrix getTransformation(double) const;
	//! Returns the translation part of the homogeneous matrix.
	Vector3d getTranslation(double) const;

private:

	static std::map<std::string,Direction> StringToDirection;

	//! The direction of the rotation.
	Direction _dir;

	uint _madId;

	void _parse(const ptree&);

};

} // end namespace Geometry

} // end namespace SX

#endif /* NSXTOOL_ROTATION_H_ */
