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

#ifndef NSXTOOL_ILLASCIIMETAREADER_H_
#define NSXTOOL_ILLASCIIMETAREADER_H_

#include "Singleton.h"
#include <sstream>

namespace SX {

namespace Data {

class MetaData;

class ILLAsciiMetaReader: public Singleton<ILLAsciiMetaReader>
{
public:
	//! Reads MetaData from a chain of characters as written in legacy ILL format
	//! return a MetaData Object
	static MetaData* read(const char* buf,std::size_t& size);
private:
	//! current line in the file.
	static int _currentline;
	//! Read the file header containing the numor, user, instr, local contact, date and time
	static void readHeader(std::stringstream&,MetaData*);
	//! Read the control block containing all Integer Metadata.
	static void readControlIBlock(std::stringstream&,MetaData*);
	//! Read the control block containing all float parameters.
	static void readControlFBlock(std::stringstream&,MetaData*);
	//! Invoke seekg to beginning the line number, at position pos. First line is 1
	static inline void gotoLine(std::stringstream& buffer, int line_number,int pos);
};

} // namespace Data

} // namespace SX

#endif NSXTOOL_ILLASCIIMETAREADER_H_
