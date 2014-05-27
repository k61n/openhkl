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
#ifndef NSXTOOL_ILLASCIIREADER_H_
#define NSXTOOL_ILLASCIIREADER_H_

#include <string>
#include <vector>

#include <boost/interprocess/file_mapping.hpp>

#include "IReader.h"
#include "MetaData.h"

namespace SX {

namespace Data {

typedef unsigned int uint;

class ILLAsciiReader : public IReader
{
public:
	ILLAsciiReader(std::string& filename);
	//! Read block number i. First frame starts at index 0
	std::vector<int> getFrame(uint i) const;
	//! Read the header of the file made of MetaData.
	MetaData* getMetaData() const;
	//! Return the number of frames
	uint nFrames() const;
	virtual ~ILLAsciiReader();
private:
	void mapFile(const std::string& filename);

	std::size_t _nframes;
	std::size_t _datapoints;
	std::size_t _nangles;
	std::size_t _header_size;
	std::size_t _skipchar;
	std::size_t _datalength;

	boost::interprocess::file_mapping _map;

};

} // end namespace Data

} // end namespace SX

#endif // NSXTOOL_ILLASCIIREADER_H_
