//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/parser/Parser.h
//! @brief     Declares functions that read vectors of numbers
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef BASE_PARSER_PARSER_H
#define BASE_PARSER_PARSER_H

#include <vector>
#include <string>
#include <sstream>

namespace nsx {

//! Method to read a vector of numeric values from two char pointers
template<class T>
void readNumFromChar(const char* begin, const char* end, std::vector<T>& v)
{
    std::istringstream istr(std::string(begin, end));
    while(!istr.eof())
    {
        T num{};
        istr >> num;

		if(istr.fail())
			break;
        
		v.push_back(num);
    }
}

} // namespace nsx

#endif // BASE_PARSER_PARSER_H
