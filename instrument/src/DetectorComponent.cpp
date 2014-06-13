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

#include <boost/property_tree/ptree.hpp>

#include "DetectorComponent.h"

namespace SX
{

namespace Instrument
{

using namespace boost::property_tree;

Component* DetectorComponent::create(const ptree& pt)
{
	return new DetectorComponent(pt);
}

DetectorComponent::DetectorComponent(const ptree& pt) : Component(), _nrows(0), _ncols(0), _width(0.0), _height(0.0), _shape(planar), _origin(bottom_left), _order(column_major)
{
	parse(pt);
}

void DetectorComponent::parse(const ptree& p)
{
	std::cout<<"I PARSE A DETECTOR"<<std::endl;
}

DetectorComponent::~DetectorComponent()
{
}

} // End namespace Instrument

} // End namespace SX

