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

#include <iostream>

#include <boost/foreach.hpp>

#include "Component.h"
#include "Modifier.h"

namespace SX
{

namespace Instrument
{

Component::Component() : _modifier(nullptr), _name(), _position()
{
}

Component::~Component()
{
}

const std::string& Component::getName() const
{
	return _name;
}

void Component::parse(const ptree& node)
{

	// A component must have a name in its XML node.
	_name=node.get<std::string>("name");

	// A component may have a x, y and z position in their corresponding XML node. If not, their position is set to the origin.
	_position[0] = node.get<double>("position.<xmlattr>.x",0.0);
	_position[1] = node.get<double>("position.<xmlattr>.y",0.0);
	_position[2] = node.get<double>("position.<xmlattr>.z",0.0);

	// Loop over the inner nodes to detect any "modifier" node. There must at most one such node.
	BOOST_FOREACH(ptree::value_type v, node)
	{
		if (v.first == "modifier")
		{
			if (_modifier == nullptr)
				_modifier=new Modifier(v.second);
		}
	}

	// Parse the XML nodes that are specific to the components.
	_parse(node);
}

} // end namespace Instrument

} // end namespace SX

