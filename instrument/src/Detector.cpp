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
#include <stdexcept>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Detector.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Units;

Detector::Detector()
: Component(), _nRows(0), _nCols(0), _rowMin(0), _colMin(0), _rowMax(0), _colMax(0), _width(0.0), _height(0.0), _pixelWidth(0.0), _pixelHeight(0.0)
{
}

Detector::Detector(const ptree& pt) : Component()
{
	parse(pt);
}

double Detector::getHeigth() const
{
	return _height;
}

uint Detector::getNCols() const
{
	return _nCols;
}

uint Detector::getNRows() const
{
	return _nRows;
}

double Detector::getPixelHeigth() const
{
	return _pixelHeight;
}

double Detector::getPixelWidth() const
{
	return _pixelWidth;
}

double Detector::getWidth() const
{
	return _width;
}

Vector3d Detector::getQVector(uint px, uint py) const
{

}

bool Detector::hasPixel(uint px, uint py) const
{
	return (px>=_rowMin) & (px<_rowMax) & (py>=_colMin) & (py<_colMax);
}

void Detector::_parse(const ptree& node)
{

	UnitsManager* unitManager=UnitsManager::Instance();

	_nRows=node.get<uint>("nrows");
	if (_nRows<=0)
		throw std::runtime_error("The number of rows of a detector must be a strictly positive number.");
	_rowMin=node.get<uint>("row_min",0);
	_rowMax = _rowMin + _nRows;

	_nCols=node.get<uint>("ncols");
	if (_nRows<=0)
		throw std::runtime_error("The number of columns of a detector must be a strictly positive number.");
	_colMin=node.get<uint>("col_min",0);
	_colMax = _colMin + _nCols;

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
	_pixelHeight=_height/_nRows;

}

Detector::~Detector()
{
}

} // End namespace Instrument

} // End namespace SX

