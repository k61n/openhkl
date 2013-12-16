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

#ifndef NSXTOOL_DATA3D_H_
#define NSXTOOL_DATA3D_H_

#include <initializer_list>
#include <string>
#include <utility>

#include "MMILLAsciiReader.h"
#include "IData.h"

namespace SX
{

namespace Data
{

typedef unsigned int uint;

template<typename T>
class Data3D : public IData<T,3>
{
public:

	Data3D();

	~Data3D();

	T get(const std::initializer_list<uint>& indices) const;

	void read(const std::string& filename);

};

template<typename T>
Data3D<T>::Data3D() : IData<T,3>()
{
}

template<typename T>
Data3D<T>::~Data3D()
{
}

template<typename T>
void Data3D<T>::read(const std::string& filename)
{

	// Read the file using memory map
    MMILLAsciiReader mm(filename.c_str());
    this->_meta=mm.readMetaDataBlock();

    this->_nFrames = mm.nBlocks();

    this->_frames.resize(this->_nFrames);

//    #pragma omp parallel for
//    for (std::size_t i=0;i<mm.nBlocks();++i)
//	    this->_frames[i]=std::move(mm.readBlock(i));
}

template<typename T>
T Data3D<T>::get(const std::initializer_list<uint>& indices) const
{
	return 1.0;
}

} // namespace Data

} // namespace SX

#endif // NSXTOOL_DATA3D_H_
