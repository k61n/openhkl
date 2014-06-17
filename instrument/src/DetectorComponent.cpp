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
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include "DetectorComponent.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Units;

std::unordered_map<std::string,DetectorComponent::shape> DetectorComponent::shapeMap=map_list_of("planar",shape::PLANAR)("cylindrical",shape::CYLINDRICAL);

std::unordered_map<std::string,DetectorComponent::layout> DetectorComponent::layoutMap=map_list_of("by_column",layout::BY_COLUMN)("by_row",layout::BY_ROW);

Component* DetectorComponent::create(const ptree& pt)
{
	return new DetectorComponent(pt);
}

DetectorComponent::DetectorComponent(const ptree& pt) : Component()
{
	parse(pt);
}

void DetectorComponent::parse(const ptree& node)
{

	UnitsManager* unitManager=UnitsManager::Instance();

	_name=node.get<std::string>("name");

	_nRows=node.get<uint>("nrows");
	if (_nRows<=0)
		throw std::runtime_error("The number of rows of a detector must be a strictly positive number.");

	_nCols=node.get<uint>("ncols");
	if (_nRows<=0)
		throw std::runtime_error("The number of columns of a detector must be a strictly positive number.");

	_startIndex=node.get<uint>("start_index",0);

	_width=node.get<double>("width");
	if (_width<=0)
		throw std::runtime_error("The width of a detector must be a strictly positive number.");
	std::string unit =node.get_child("width.<xmlattr>").get<std::string>("units");
	_width *= unitManager->get(unit);

	_pixelWidth=_width/_nCols;

	_height=node.get<double>("height");
	if (_height<=0)
		throw std::runtime_error("The height of a detector must be a strictly positive number.");
	unit =node.get_child("height.<xmlattr>").get<std::string>("units");
	_height *= unitManager->get(unit);

	_pixelWidth=_height/_nRows;

	auto it=shapeMap.find(node.get<std::string>("shape","planar"));
	if (it==shapeMap.end())
		throw std::runtime_error("Invalid detector shape.");
	_shape=it->second;

	auto it1=layoutMap.find(node.get<std::string>("layout","by_row"));
	if (it1==layoutMap.end())
		throw std::runtime_error("Invalid data layout.");
	_layout=it1->second;

	_mapping = new DetectorMapping(_nRows,_nCols,_startIndex,static_cast<bool>(_layout));

}

DetectorComponent::~DetectorComponent()
{
	delete _mapping;
}

} // End namespace Instrument

} // End namespace SX

