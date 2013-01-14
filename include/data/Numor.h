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

#ifndef  NSXTOOL_NUMOR_H_
#define NSXTOOL_NUMOR_H_
#include <string>
#include <sstream>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "MetaData.h"

namespace SX {

//! Class for  Single Crystal Numors.
// Numors contains a unique identifier, data and
// metadata. The file is parsed and all the metadata
// are added by default to the numor. If a metadata field
// does not already exist as an available key, it is added to the MetaKeys
// that maintains a set of all available keys.
//
class Numor {
public:
	//! Construct a numor from its mad file.
	Numor()
	{}
	//! Construct a numor from its file.
	Numor(const std::string& filename);
	Numor(const std::string& filename,const Numor& tocopy);
	//! Copy constructor
	Numor(const Numor&);
	Numor& operator=(const Numor&);
	 ~Numor();
	 //! Get the number
	 int getNo() const;
	 //! Get MetaData from this numor
	 //@return : Metadata
	 const MetaData& getMetaData() const;
	 //! Return the raw data (list of value in a 1D-vector)
	 const std::vector<double>& getData() const;
	 //! Is bpb
	 bool isbpb() const;
private:
	 //! Read the file header containing the numor, user, instr, local contact, date and time
	 inline void readHeader(std::stringstream&);
	 //! Read the control block containing all Integer Metadata.
	 inline void readControlIBlock(std::stringstream&);
	 //! Read the control block containing all float parameters.
	 inline void readControlFBlock(std::stringstream&);
	 inline void readData(std::stringstream&);
	 //! Invoke seekg to beginning the line number, at position pos. First line is 1
	 static inline void gotoLine(std::stringstream& buffer, int line_number,int pos);
	//! Validate text line with 80 characters such as AAAA., RRRR., etc...
	static inline bool validLine(const std::string& line,const char* letter);
	//! The numor
	int _number;
	//! Metadata of the numor
	MetaData _meta;
	//! current line in the file.
	static int _currentline;
	//! the raw data consisting of npoints read in sequence.
	std::vector<double> _rawdata;
};

} /* namespace SX */
#endif /* NSXTOOL_NUMOR_H_ */
