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

#ifndef NSXTOOL_AXIS_H_
#define NSXTOOL_AXIS_H_
#include <string>
#include <Eigen/Dense>
#include <Eigen/Geometry>

using Eigen::Vector3d;
using Eigen::Transform;

namespace SX {
namespace Instrument{


class Axis {
public:
	Axis(const std::string& label);
	virtual ~Axis()=0;
	//! Give a label to this axis
	void setLabel(const std::string& label);
	//! Return the axis label
	const std::string& getLabel() const;
	//! Give the direction of the axis.
	//! Axis is normalized
	void setAxis(const Vector3d& axis);
	//! Get the normalized direction of this axis
	const Vector3d& getAxis() const;
	//! Set the current offset
	void setOffset(double offset);
	//! Add an offset to the existing one
	void addOffset(double offset);
	//! Return the value of the offset
	double getOffset() const;
	//! Set the range of values accessible for this axis
	void setLimits(double, double);
	//! Set the  max value of the range
	void setMax(double);
	//! Set the min value of the range
	void setMin(double);
	//! Get minimum of the range
	double getMin() const;
	//! Get maximum of the range
	double getMax() const;
	//! Get the homogeneous (4x4) matrix corresponding to the value
	virtual Transform<double,3,Eigen::Affine> getHomMatrix(double value) const=0;
	//! Transform vector
	Vector3d transform(double value, const Vector3d& v);
protected:
	//! Label of the axis
	std::string _label;
	//! Axis direction, a normalized vector
	Vector3d _axis;
	//! Offset so that finalvalue=offset+given value
	double _offset;
	//! Limits
	double _min, _max;
};

} // End namespace Instrument
} // End namespace SX
#endif /* NSXTOOL_AXIS_H_ */
