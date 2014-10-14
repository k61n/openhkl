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

#ifndef SX_GONIO_H_
#define SX_GONIO_H_
#include <string>
#include <vector>
#include <utility>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "Axis.h"
#include "RotAxis.h"

namespace SX {

namespace Instrument {

using Eigen::Vector3d;
/* !
 * \brief Class Gonio.
 * Base class for all goniometers (system of several rotation axis). Any number of axis can be used.
 * The rotation is applied in reverse order of the given order. For example, when defining a goniometer with
 * 3 axes a,b,c, it is supposed that in such collection of axes, b is attached to the a shaft and c is attached to the
 * b shaft. Such gonio will rotate a vector v into a.b.c.v.
 * Once the Gonio is constructed, it is not allowed to change the number of axes.
 * Axes, their labels and respective limits can be modified by the class methods or by template accessor:
 * e.g. Axis<0>(g)=RotAxis(UnitZ,CW).
 */
class Gonio {
public:
	//! Initialize an empty Gonio with naxes
	Gonio(const std::string& label);
	//! Destructor
	~Gonio();
	Axis* addRotation(const std::string& label,const Vector3d& axis, RotAxis::Direction dir, double offset=0.0, bool physical=true);
	Axis* addTranslation(const std::string& label,const Vector3d& axis, bool physical=true);
	//! Pointer to axis i, throw range_error if not found
	Axis* const getAxis(unsigned int i);
	//! Pointer to axis with label, throw range_error if not found
    Axis* const getAxis(const std::string& label);
	//! Return the homogeneous matrix corresponding to this set of parameters. Throw if angles outside limits.
	Eigen::Transform<double,3,Eigen::Affine> getHomMatrix(const std::vector<double>& values) const;
	//! Return the inverse of the homogeneous matrix corresponding to this set of parameters. Throw if angles outside limits.
	Eigen::Transform<double,3,Eigen::Affine> getInverseHomMatrix(const std::vector<double>& values) const;
	//! Transform a point in 3D space, given a vector of parameters
	Vector3d transform(const Vector3d& v,const std::vector<double>& values);
	Vector3d transformInverse(const Vector3d& v,const std::vector<double>& values);
	//! Transform a vector inplace, for a values of Gonio parameters
	void transformInPlace(Vector3d& v,const std::vector<double>& values);
	void transformInverseInPlace(Vector3d& v,const std::vector<double>& values);
	//! Get the number of Axis
	std::size_t numberOfAxes() const;
	//! Reset all offsets
	void resetOffsets();
	//! Return the number of physical axis defined in the gonio
	std::size_t nPhysicalAxis() const;
protected:
	//! Given name of the gonio
	std::string _label;
	//! Check whether axis i within the range of Axis
	void isAxisValid(unsigned int i) const;
	//! Check whether s names a valid axis
	unsigned int isAxisValid(const std::string& s) const;
	//! Set of axis
	std::vector<Axis*> _axes;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


} // End namespace Geometry
} // End namespace SX

#endif /* SX_GONIO_H_ */
