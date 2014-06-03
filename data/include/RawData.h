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

#ifndef NSXTOOL_RAWDATA_H_
#define NSXTOOL_RAWDATA_H_
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <memory>
#include "MetaData.h"
#include "DataReaderFactory.h"


namespace SX {
namespace Data {

class Instrument;

class RawData {
	enum accessType {InMemory=1,OnDisk=2};
public:
	RawData(const std::string& filename, const std::string& filetype, accessType access)
{
//		DataReaderFactory* factory=DataReaderFactory::Instance();
//		_reader=factory->create(filetype);
//		_reader->open(filename);
//		_metadata=std::unique_ptr<MetaData>(_reader->getMetaData());
//		if (access==InMemory)
//		{
//			_frames=_reader->nFrames();
//			_data.resize(_frames);
//
//		}

}
	virtual ~RawData();
private:
	IDataReader* _reader;
	accessType _access;
	std::shared_ptr<Instrument> _instr;
	std::unique_ptr<MetaData> _metadata;
	std::vector<Eigen::MatrixXi> _data;
	int _frames;
};

}
}
#endif /* NSXTOOL_RAWDATA_H_ */
