/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
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

#ifndef NSXLIB_PARSER_H
#define NSXLIB_PARSER_H

#include <vector>

namespace nsx {

//! Method to read a vector of int values from two char pointers, using spirit
//! This is faster than the C atoi() function.
void readIntsFromChar(const char* begin, const char* end, std::vector<int>& v);

//! Method to read a vector of double values from two char pointers, using spirit
void readDoublesFromChar(const char* begin, const char* end, std::vector<double>& v);

} // end namespace nsx

#endif // NSXLIB_PARSER_H
