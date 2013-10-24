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

#ifndef NSXTOOL_ROTAXIS_H_
#define NSXTOOL_ROTAXIS_H_
#include "V3D.h"
#include "Quat.h"
#include "Matrix33.h"
#include <iostream>

namespace SX
{

namespace Geometry
{
	enum Direction {CCW=0,CW=1};

/* !
 * \brief Class defining a rotation axis.
 *
 *  Allows to define a rotation axis in 3D space , constructed by a direction vector
 *  and a rotation direction (CW or CCW). The direction vector needs not to be normalized.
 *
 */
class RotAxis
{
public:
	//! Constructor per default, rotation around z, CCW
	RotAxis();
	//! Explicit
	RotAxis(const V3D& axis, Direction direction=CCW);
	//! Destructor
	~RotAxis();
	//! Set the axis. V3D will be normalized.
	void setAxis(const V3D&);
	//! Get the rotation axis
	const V3D& getAxis() const;
	//! Get the rotation axis
	V3D& getAxis();
	//! get rotation direction.
	void setRotationDirection(Direction);
	//! Get the rotation matrix associated with this rotation
	//@param angle : rotation angle in radians by default, use Units to convert
	//@return rotation matrix
	Matrix33<double> getMatrix(double angle) const;
	//! Get the quaternion associated with this rotation
	//@param angle : rotation angle in radians by default
	//@return rotation matrix
	Quat getQuat(double angle) const;
	//! Print information into a stream
	friend std::ostream& operator<<(std::ostream& os, const RotAxis&);
protected:
	//! Axis of rotation, normalized vector
	V3D _axis;
	//! Rotation direction
	Direction _dir;
};

	static const RotAxis AxisXCW=RotAxis(V3D(1,0,0),CW);
	static const RotAxis AxisXCCW=RotAxis(V3D(1,0,0),CCW);
	static const RotAxis AxisYCW=RotAxis(V3D(0,1,0),CW);
	static const RotAxis AxisYCCW=RotAxis(V3D(0,1,0),CCW);
	static const RotAxis AxisZCW=RotAxis(V3D(0,0,1),CW);
	static const RotAxis AxisZCCW=RotAxis(V3D(0,0,1),CCW);

} //namespace Geometry

} // End namespace SX

#endif /* NSXTOOL_ROTAXIS_H_ */
