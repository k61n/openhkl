//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Path.h
//! @brief     Declares functions that operate on paths
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_UTILS_PATH_H
#define NSX_BASE_UTILS_PATH_H

#include <vector>

namespace ohkl {

//! Returns the file separator for the running OS
std::string fileSeparator();

//! Returns the basename of a given path
std::string fileBasename(const std::string& input_path);

//! Returns a path stripped off its file extension
std::pair<std::string, std::string> splitFileExtension(const std::string& input_path);

//! Gets the directory name of a given path
std::string fileDirname(const std::string& input_path);

//! Create a directory. The directory can be nested (0x1C0 is 0700)
void makeDirectory(const std::string& path, int mode = 0x1C0);

std::string buildPath(const std::vector<std::string>& paths, const std::string& root = "");

//! Returns the home directory
std::string homeDirectory();

//! Returns the OpenHKL application data path
std::string applicationDataPath();

//! Returns true if the file exists
bool fileExists(const std::string& filename);

std::string diffractometersPath();

//! Returns a temporary filename based on a given filename
std::string tempFilename(const std::string& filename);

} // namespace ohkl

#endif // NSX_BASE_UTILS_PATH_H
