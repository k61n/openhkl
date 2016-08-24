/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr
 j.fisher[at]fz-juelich.de

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

#ifndef NSXTOOL_CSV_H_
#define NSXTOOL_CSV_H_

#include <vector>
#include <iostream>
#include <string>

namespace SX {

namespace Utils {

//! @class CSV
//! @brief Simple CSV parser
class CSV {
public:
    CSV(char delim=',', char quotchar='"');
    ~CSV();

    std::vector<std::string> getRow(std::istream& stream);

private:
    char _delim;
    char _quotchar;

    std::string removePadding(const std::string& str); //!<< Remove leading and trailing whitespace
    std::string getToken(std::istream& stream); //!<< Get next token
    std::string skipWhitespace(std::istream& stream); //!<< Skip to first non-whitespace character
};

} // namespace Utils

} // namespace SX

#endif // NSXTOOL_CSV_H_
