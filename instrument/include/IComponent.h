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

#ifndef NSXTOOL_ICOMPONENT_H_
#define NSXTOOL_ICOMPONENT_H_
#include <Eigen/Dense>

namespace SX
{
namespace Instrument
{

class IComponent
{
public:

	IComponent(const std::string& name);
	virtual ~IComponent();
	//!
	virtual void toXML(std::ostream&);
	//!
	virtual void fromXML(std::istream&);

	void setPos(const Eigen::Vector3d& pos,const Eigen::Vector3d& temp=Eigen::Vector3d::Constant(0.0));
private:
	std::string _name;
	// Rest position of a component
	Eigen::Vector3d& _pos;
	// Error in the rest position
	Eigen::Vector3d& _posSigma;
};

}
}
#endif /* NSXTOOL_ICOMPONENT_H_ */
