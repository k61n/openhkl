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

#ifndef NSXTOOL_DETECTOR_H_
#define NSXTOOL_DETECTOR_H_

#include <unordered_map>
#include <string>

#include <boost/assign/list_of.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Eigen/Dense>

#include "Component.h"
#include "DetectorMapping.h"

namespace SX
{

namespace Instrument
{

typedef unsigned int uint;

using namespace boost::assign;
using namespace boost::property_tree;
using namespace Eigen;

class Detector : public Component
{
public:

	//! Enumerates the possible detector shapes.
	enum class shape {PLANAR=1,CYLINDRICAL=2};

	enum class layout {BY_COLUMN=false,BY_ROW=true};

	static std::unordered_map<std::string,shape> shapeMap;

	static std::unordered_map<std::string,layout> layoutMap;

	static Component* create(const ptree& pt);

	Vector3d getQVector(uint px, uint py) const;

	bool hasPixel(uint px, uint py) const;

	virtual void parse(const ptree& pt);

	virtual ~Detector();

protected:

	Detector();

	Detector(const ptree& pt);

	uint _nRows, _nCols;
	uint _rowMin,_colMin, _rowMax, _colMax;
	double _width, _height;
	double _pixelWidth, _pixelHeight;
	shape _shape;
	layout _layout;

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_DETECTOR_H_ */
