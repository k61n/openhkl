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

#ifndef NSXTOOL_PRIMITIVETRANSFORMATION_H_
#define NSXTOOL_PRIMITIVETRANSFORMATION_H_

#include <boost/property_tree/ptree.hpp>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "XMLConfigurable.h"

namespace SX
{

namespace Geometry
{

using boost::property_tree::ptree;

using Eigen::Affine;
using Eigen::Matrix3d;
using Eigen::Transform;
using Eigen::Vector3d;

using SX::Kernel::XMLConfigurable;

typedef Transform<double,3,3,Affine> HomMatrix;

/*
 * Defines an interface for primitive transformations (e.g. Rotation, Scaling, Translation) that will be the building block for
 * any homogeneous transformation.
 */
class PrimitiveTransformation : public XMLConfigurable
{
public:

	static PrimitiveTransformation* Create(const ptree&);

	//! Default constructor.
	PrimitiveTransformation();
	//! Explicit constructor.
	PrimitiveTransformation(const Vector3d&, double);
	//! Constructor from an XML node.
	PrimitiveTransformation(const ptree&);
	//! Destructor.
	virtual ~PrimitiveTransformation()=0;

	//! Returns the rotation part of the homogeneous matrix.
	virtual Matrix3d getRotation(double) const=0;
	//! Returns the homogeneous matrix.
	virtual HomMatrix getTransformation(double) const=0;
	//! Returns the translation part of the homogeneous matrix.
	virtual Vector3d getTranslation(double) const=0;

	//! Get the rotation axis.
	const Vector3d& getAxis() const;
	//! Get the rotation axis.
	Vector3d& getAxis();
	//! Get the angular offset of this axis (radians).
	double getOffset() const;
	void parse(const ptree&);
	//! Set the transformation axis.
	void setAxis(const Vector3d&);
	//! Set the transformation offset.
	void setOffset(double);

protected:

	std::string _name;

	//! The transformation axis.
	Vector3d _axis;

	//! The offset (angle for rotation, distance for translation).
	double _offset;

	virtual void _parse(const ptree&)=0;

};

} // end namespace Geometry

} // end namespace SX

#endif /* NSXTOOL_PRIMITIVETRANSFORMATION_H_ */
