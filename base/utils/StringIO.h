//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/StringIO.h
//! @brief     Declares functions that transform strings
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_UTILS_STRINGIO_H
#define OHKL_BASE_UTILS_STRINGIO_H

namespace ohkl {

std::string clear_spaces(const std::string& input_string);

std::string trim(const std::string& input_string);

std::string compress(const std::string& input_string);

std::string join(const std::vector<std::string>& tokens, std::string delimiter = " ");

std::vector<std::string> split(const std::string& input_string, const std::string& delimiter = " ");

std::string lowerCase(const std::string& str_in);

//! Returns current date and time as a string
std::string datetime_str();

} // namespace ohkl

#endif // OHKL_BASE_UTILS_STRINGIO_H
