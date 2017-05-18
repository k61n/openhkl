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

#ifndef NSXTOOL_TRANSAXIS_H_
#define NSXTOOL_TRANSAXIS_H_

#include <Eigen/Geometry>

#include "Axis.h"

namespace nsx {

class TransAxis : public Axis {
public:

	//! Static constructor for a TransAxis
	static Axis* create(const proptree::ptree& node);

	//! Default constructor
	TransAxis();
	//! Copy constructor
	TransAxis(const TransAxis& other);
	//! Constructs a translation axis with a given label
	TransAxis(const std::string& label);
	//! Constructs a translation axis with a given label and axis
	TransAxis(const std::string& label,const Vector3d& axis);
	//! Construct a TransAxis from a property tree node.
	TransAxis(const proptree::ptree& node);
	// Destructor
	~TransAxis();
	//! Virtual copy constructor
	TransAxis* clone() const;

	//! Assignment operator
	TransAxis& operator=(const TransAxis& other);
	Eigen::Transform<double,3,Eigen::Affine> getHomMatrix(double value) const;

};

} // end namespace nsx

#endif /* NSXTOOL_TRANSAXIS_H_ */
