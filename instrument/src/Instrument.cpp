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

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Instrument.h"
#include "ComponentFactory.h"

namespace SX
{

namespace Instrument
{

using namespace boost::property_tree;

Instrument::Instrument()
{
}

void Instrument::add(Component* comp)
{
	std::string compName = comp->getName();
	// Check that the component name is not already used. If not add it to the components vector.
	for (auto it=_components.begin();it!=_components.end();++it)
	{
		if (compName == (*it)->getName())
			throw std::runtime_error("Component name "+compName+" already in use.");
	}
	_components.push_back(comp);
}

void Instrument::load(const std::string& instrFile)
{
	std::ifstream is(instrFile);
	if (is.is_open())
	{
		ptree pt;
		read_xml(is, pt);
		parse(pt);
		is.close();
	}
}

void Instrument::parse(const ptree& pt)
{
	// Call (or create) an instance of the instrument component factory.
	ComponentFactory* compFactory = ComponentFactory::Instance();

	// Loop over the "component" nodes of the XML file.
	BOOST_FOREACH(ptree::value_type v, pt.get_child("instrument"))
	{
		if (v.first == "component")
		{
			// Fetch the "type" component node attribute and get the corresponding Component object from the component factory.
			std::string cType=v.second.get_child("<xmlattr>").get<std::string>("type");
			Component* comp=compFactory->create(cType,v.second);
			add(comp);
		}
	}
}

} // end namespace Instrument

} // end namespace SX

