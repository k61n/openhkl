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

#ifndef NSXTOOL_DETECTORCOMPONENT_H_
#define NSXTOOL_DETECTORCOMPONENT_H_

#include <boost/property_tree/ptree.hpp>

#include <Eigen/Dense>

#include "Component.h"

namespace SX
{

namespace Instrument
{

typedef unsigned int uint;

using namespace boost::property_tree;

class DetectorComponent : public Component
{
public:

	//! Enumerates the possible orders of data storage in memory.
	enum dataOrder {column_major=1,row_major=2};

	//! Enumerates the possible positions of the origin of the detector.
	enum detectorOrigin {bottom_left=1,bottom_right=2,top_left=3,top_right=4};

	//! Enumerates the possible detector shapes.
	enum detectorShape {planar=1,cylindrical=2,flat_cone=3};

	static Component* create(const ptree& pt);

	void parse(const ptree& pt);

	virtual ~DetectorComponent();

protected:

	DetectorComponent();

	DetectorComponent(const ptree& pt);

	uint _nrows, _ncols;
	double _width, _height;
	dataOrder _order;
	detectorOrigin _origin;
	detectorShape _shape;

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_DETECTORCOMPONENT_H_ */
