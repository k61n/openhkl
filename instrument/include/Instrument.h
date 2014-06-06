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

#ifndef NSXTOOL_INSTRUMENT_H_
#define NSXTOOL_INSTRUMENT_H_

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "IComponent.h"
#include "Composite.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Kernel;
using boost::property_tree::ptree;

class Instrument : Composite<IComponent>
{
public:
	Instrument(const std::string& instrFile);
	void load(const ptree& pt);
	int nComponents() const;
};

Instrument::Instrument(const std::string& instrFile)
{
	std::ifstream is(instrFile);
	if (is.is_open())
	{
		ptree pt;
		read_xml(is, pt);
		load(pt.get_child("instrument"));
	}
}

void Instrument::load(const ptree& pt)
{
}

int Instrument::nComponents() const
{
	return _components.size();
}


} // end namespace Instrument

} // end namespace SX

#endif /* INSTRUMENT_H_ */
