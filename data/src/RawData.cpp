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

#include "RawData.h"

namespace SX
{

namespace Data
{

RawData::RawData(const std::string& filename, const std::string& filetype, accessType access) : _access(access)
{
	// Open the filename using the adequate reader.
	DataReaderFactory* factory=DataReaderFactory::Instance();
	_reader=std::unique_ptr<IDataReader>(factory->create(filetype));
	_reader->open(filename);

	// Get the metadata.
	_metadata=std::unique_ptr<MetaData>(_reader->getMetaData());
	if (access==InMemory)
	{
		_frames=_reader->nFrames();
		_data.resize(_frames);

	}

	// Instanciate the instrument object that matches the instrument name found in the metadata.
	std::string instrumentName=_metadata->getKey<std::string>("Instrument");
	InstrumentStore* instrStore=InstrumentStore::Instance();
	_instrument = instrStore->get(instrumentName);
}

RawData::~RawData()
{
}

} // End namespace Data

} // End namespace SX

