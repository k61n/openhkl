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

#ifndef NSXTOOL_GONIOSTATE_H_
#define NSXTOOL_GONIOSTATE_H_
#include <vector>
#include <Eigen/Geometry>

namespace SX {
namespace Instrument{

// Forward declare
class Gonio;

//! Maintain a state of a goniometer, following the memento pattern.
class GonioState {
public:
	const std::vector<double>& getValues() const;
	~GonioState();
private:
	//! Only Gonio class can create a state
	GonioState();
	//! Gonio must be able to access GonioState
	friend class Gonio;
	//! Pointer to the Goniometer that has created the state
	const Gonio* _gonio;
	//! Values for each axis of the Goniometer
	std::vector<double> _values;
	//! Homogeneous transformation matrix corresponding to this state
	Eigen::Transform<double,3,Eigen::Affine> _transformation;
};

}
}
#endif /* NSXTOOL_GONIOSTATE_H_ */
