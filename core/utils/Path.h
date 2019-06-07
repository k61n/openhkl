#ifndef CORE_UTILS_PATH_H
#define CORE_UTILS_PATH_H

#include <string>
#include <utility>
#include <vector>

namespace nsx {

//! Returns the file separator for the running OS
std::string fileSeparator();

//! Returns the basename of a given path
std::string fileBasename(const std::string& input_path);

//! Returns a path stripped off its file extension
std::pair<std::string, std::string> splitFileExtension(const std::string& input_path);

//! Get the directory name of a given path
std::string fileDirname(const std::string& input_path);

//! Create a directory. The directory can be nested (0x1C0 is 0700)
void makeDirectory(const std::string& path, int mode = 0x1C0);

std::string buildPath(const std::vector<std::string>& paths, const std::string& root = "");

//! Returns the home directory
std::string homeDirectory();

//! Returns the NSXTool application data path
std::string applicationDataPath();

//! Return true if the file exists
bool fileExists(const std::string& filename);

std::string diffractometersPath();

} // end namespace nsx

#endif // CORE_UTILS_PATH_H
