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

#include "MultiDetector.h"

namespace SX
{

namespace Instrument
{

const Detector* MultiDetector::findDetector(uint px, uint py) const
{

	for (component_const_iterator it=_components.begin();it!=_components.end();++it)
	{
		if ((*it)->hasPixel(px,py))
			return *it;
	}
	return nullptr;
}

Vector3d MultiDetector::getQVector(uint px, uint py)
{
	for (auto it=_components.begin();it!=_components.end();++it)
	{
		if ((*it)->hasPixel(px,py))
			return (*it)->getQVector(px,py);
	}
}

bool MultiDetector::hasPixel(uint px, uint py) const
{
	for (auto it=_components.begin();it!=_components.end();++it)
	{
		if ((*it)->hasPixel(px,py))
			return true;
	}
	return false;
}

MultiDetector::~MultiDetector()
{
}


} // end namespace Instrument

} // end namespace SX

