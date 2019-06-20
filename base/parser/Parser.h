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

namespace nsx {

//! Method to read a vector of int values from two char pointers, using spirit
//! This is faster than the C atoi() function.
void readIntsFromChar(const char* begin, const char* end, std::vector<int>& v);

//! Method to read a vector of double values from two char pointers, using spirit
void readDoublesFromChar(const char* begin, const char* end, std::vector<double>& v);

} // namespace nsx

#endif // BASE_PARSER_PARSER_H
