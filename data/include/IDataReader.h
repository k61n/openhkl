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

#ifndef NSXTOOL_IDATAREADER_H_
#define NSXTOOL_IDATAREADER_H_
#include <string>
#include <vector>

#include "MetaData.h"

namespace SX
{

namespace Data
{

typedef unsigned int uint;

class IDataReader
{
public:
	static IDataReader* create();
	//! Return the 1D vectors of counts for a given frame.
	virtual void open(const std::string& filename) = 0;
	virtual std::vector<int> getFrame(uint i) const=0;
	//! Return the metadata.
	virtual MetaData* getMetaData()=0;
	//! Returns the number of frames.
	virtual uint nFrames() const=0;
	//! The virtual abstract destructor.
	virtual ~IDataReader()=0;
protected:
	MetaData* _metadata;
	IDataReader();
	std::string _filename;
};


} // end namespace Data

} // end namespace SX

#endif /* NSXTOOL_IDATAREADER_H_ */
