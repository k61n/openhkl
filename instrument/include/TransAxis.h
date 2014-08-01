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
#include "Axis.h"
#include <Eigen/Geometry>

namespace SX {
namespace Instrument {



class TransAxis : public Axis
{
public:
	TransAxis(const std::string& label);
	TransAxis(const std::string& label,const Vector3d& axis);
	~TransAxis();
	Eigen::Transform<double,3,Eigen::Affine> getHomMatrix(double value) const;

};

} // End namespace instrument
} // End namesapce SX
#endif /* NSXTOOL_TRANSAXIS_H_ */
