//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/CSV.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_UTILS_CSV_H
#define CORE_UTILS_CSV_H

#include <iostream>
#include <string>
#include <vector>

namespace nsx {

//! @class CSV
//! @brief Simple CSV parser
class CSV {
public:
    CSV(char delim = ',', char quotchar = '"');
    ~CSV();

    std::vector<std::string> getRow(std::istream& stream);

private:
    char _delim;
    char _quotchar;

    std::string getToken(
        std::istream& stream,
        char& delim); //!<< Get next token, write delimeter type
};

} // namespace nsx

#endif // CORE_UTILS_CSV_H
