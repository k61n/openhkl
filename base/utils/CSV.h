//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/CSV.h
//! @brief     Defines class CSV
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_UTILS_CSV_H
#define NSX_BASE_UTILS_CSV_H

#include <iostream>
#include <vector>

namespace ohkl {

//! Simple CSV parser.

class CSV {
 public:
    CSV(char delim = ',', char quotchar = '"');

    std::vector<std::string> getRow(std::istream& stream);

 private:
    char _delim;
    char _quotchar;

    std::string getToken(std::istream& stream, char& delim) const;
};

} // namespace ohkl

#endif // NSX_BASE_UTILS_CSV_H
