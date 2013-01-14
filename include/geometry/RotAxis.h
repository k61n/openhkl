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
#ifndef  NSXTOOL_ROTAXIS_H_
#define NSXTOOL_ROTAXIS_H_
#include "V3D.h"
#include "Quat.h"
#include "Matrix33.h"
#include <string>

namespace SX
{

// Rotation direction
enum Rotation {CW=0,CCW=1};
enum AngleUnit {Radians=0,Degrees=1};

class RotAxis
{
public:
	//! Constructor
	RotAxis(const std::string& label,const V3D& axis, Rotation direction);
	//! Destructor
	~RotAxis();
	//! Get the label of this axis
	std::string& getLabel();
	const std::string& getLabel() const;
	//! Get the rotation matrix associated with this rotation
	//@param angle : rotation angle in radians by default
	//@return rotation matrix
	Matrix33<double> getMatrix(double angle,AngleUnit=Radians) const;
	//! Get the quaternion associated with this rotation
	//@param angle : rotation angle in radians by default
	//@return rotation matrix
	Quat getQuat(double angle,AngleUnit=Radians) const;
private:
	//! Label of the axis
	std::string _name;
	//! Axis of rotation, normalized vector
	V3D _axis;
	//! Rotation direction
	Rotation _dir;
};

} // End namespace SX

#endif /* NSXTOOL_ROTAXIS_H_ */
