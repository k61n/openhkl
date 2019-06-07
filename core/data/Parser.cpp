//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/Parser.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <boost/spirit/include/qi.hpp>

#include "Parser.h"

namespace nsx {

void readIntsFromChar(const char* begin, const char* end, std::vector<int>& v)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    qi::phrase_parse(begin, end, *qi::int_ >> qi::eoi, ascii::space, v);
}

void readDoublesFromChar(const char* begin, const char* end, std::vector<double>& v)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    qi::phrase_parse(begin, end, *qi::double_ >> qi::eoi, ascii::space, v);
}

} // end namespace nsx
