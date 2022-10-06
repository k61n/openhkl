//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/parser/Parser.h
//! @brief     Declares functions that read vectors of numbers
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_PARSER_PARSER_H
#define OHKL_BASE_PARSER_PARSER_H

#include <sstream>
#include <string>
#include <vector>

namespace ohkl {

//! Method to read a vector of numeric values from two char pointers
template <class T> void readNumFromChar(const char* begin, const char* end, std::vector<T>& v)
{
    std::istringstream istr(std::string(begin, end));
    while (!istr.eof()) {
        T num{};
        istr >> num;

        if (istr.fail())
            break;

        v.push_back(num);
    }
}

} // namespace ohkl

#endif // OHKL_BASE_PARSER_PARSER_H
