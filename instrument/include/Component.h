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

#ifndef NSXTOOL_COMPONENT_H_
#define NSXTOOL_COMPONENT_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

#include <Eigen/Dense>

#include "Composite.h"

namespace SX
{

namespace Geometry
{
	class HomogeneousTransformation;
}

namespace Instrument
{

using namespace SX::Kernel;
using boost::property_tree::ptree;

typedef unsigned int uint;

using SX::Geometry::HomogeneousTransformation;

/*
 * Interface for the components of an instrument (e.g. detector, goniometer, source ...).
 */
class Component
{
public:

	static Component* create(const ptree& pt);

	const std::string& getName() const;

	virtual ~Component()=0;

protected:

	Component();

	Component(const ptree& pt);

	//! Parse the XML component node.
	virtual void parse(const ptree& pt)=0;

	HomogeneousTransformation* _transformation;

	//! The name of the component.
	std::string _name;

	Eigen::Vector3d _position;

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_COMPONENT_H_ */
