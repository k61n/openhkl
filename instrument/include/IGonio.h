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

#ifndef SX_IGONIO_H_
#define SX_IGONIO_H_
#include <string>
#include <vector>
#include <initializer_list>
#include <utility>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "Axis.h"
#include "RotAxis.h"

namespace SX {

namespace Instrument {

using Eigen::Matrix3d;
/* !
 * \brief Class IGonio.
 * Base class for all goniometers (system of several rotation axis). Any number of axis can be used.
 * The rotation is applied in reverse order of the given order. For example, when defining a goniometer with
 * 3 axes a,b,c, it is supposed that in such collection of axes, b is attached to the a shaft and c is attached to the
 * b shaft. Such gonio will rotate a vector v into a.b.c.v.
 * Once the IGonio is constructed, it is not allowed to change the number of axes.
 * Axes, their labels and respective limits can be modified by the class methods or by template accessor:
 * e.g. Axis<0>(g)=RotAxis(UnitZ,CW).
 */
class IGonio {
public:
	//! Initialize an empty Gonio with naxes
	IGonio(const std::string& name);
	//! Destructor
	~IGonio();
	Axis* addRotation(const std::string& label,const Vector3d& axis, RotAxis::Direction dir);
	Axis* addTranslation(const std::string& label,const Vector3d& axis);
	//! Accesor to Axis number _I
	Axis* axis(unsigned int i);
	Axis* axis(const char* label);
	//! Const-accessor to Axis _I
	const Axis* axis(unsigned int i) const;
	const Axis* axis(const char* label) const;
	//! Limits as initializer_list
	//! Return the homogeneous matrix corresponding to this set of angles. Throw if angles outside limits.
	Eigen::Transform<double,3,Eigen::Affine> getHomMatrix(std::initializer_list<double> values);
	//!
protected:
	std::string _label;
	void isAxisValid(unsigned int i) const;
	unsigned int isAxisValid(const char*) const;
	std::vector<Axis*> _axes;
};


} // End namespace Geometry
} // End namespace SX

#endif /* SX_IGONIO_H_ */
