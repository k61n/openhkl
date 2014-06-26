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

#ifndef NSXTOOL_HOMOGENEOUSTRANSFORMATION_H_
#define NSXTOOL_HOMOGENEOUSTRANSFORMATION_H_

#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace SX
{

namespace Geometry
{

class PrimitiveTransformation;

using Eigen::Affine;
using Eigen::Matrix3d;
using Eigen::Transform;
using Eigen::Vector3d;

typedef Transform<double,3,3,Affine> HomMatrix;

/*
 * Defines a composite of primitive transformations.
 */
class HomogeneousTransformation
{

public:
	//! The default constructor.
	HomogeneousTransformation();

	//! The destructor.
	virtual ~HomogeneousTransformation()=0;

	//! Add a new primitive transformation.
	void addTransformation(PrimitiveTransformation*);
	//! Returns the rotation part of the homogeneous matrix build from the composition of the primitive transformations.
	Matrix3d getRotation(const std::vector<double>&) const;
	//! Returns the homogeneous matrix build from the composition of the primitive transformations.
	HomMatrix getTransformation(const std::vector<double>&) const;
	//! Returns the translation part of the homogeneous matrix build from the composition of the primitive transformations.
	Vector3d getTranslation(const std::vector<double>&) const;

private:
	std::vector<PrimitiveTransformation*> _trans;

};

} // end namespace Geometry

} // end namespace SX

#endif /* NSXTOOL_HOMOGENEOUSTRANSFORMATION_H_ */
