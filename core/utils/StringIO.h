//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/StringIO.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_UTILS_STRINGIO_H
#define CORE_UTILS_STRINGIO_H

#include <string>
#include <vector>

namespace nsx {

std::string clear_spaces(const std::string& input_string);

std::string trim(const std::string& input_string);

std::string compress(const std::string& input_string);

std::string join(const std::vector<std::string>& tokens, std::string delimiter = " ");

std::vector<std::string> split(const std::string& input_string, const std::string& delimiter = " ");

} // end namespace nsx

#endif // CORE_UTILS_STRINGIO_H
