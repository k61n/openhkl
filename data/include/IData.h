/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
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

#ifndef NSXTOOL_IDATA_H_
#define NSXTOOL_IDATA_H_

#include <string>
#include <vector>

#include "MMILLAsciiReader.h"

namespace SX
{

namespace Data
{

typedef unsigned int uint;

template<typename T>
class IData
{
public:

	IData();

	virtual ~IData();

	virtual void read(const std::string& filename)=0;

private:

	int _nFrames;
	std::vector<T> _frames;
	SX::Data::MetaData* _meta;

};

template<typename T>
IData<T>::IData() : _meta(nullptr), _nFrames(0)
{
	_frames.reserve(0);
}

template<typename T>
IData<T>::~IData()
{
	delete _meta;
}

} //namespace Data

} // namespace SX

#endif // NSXTOOL_DATA3D_H_
