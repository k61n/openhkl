//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/utils/TestPath.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <string>

#include "base/utils/Path.h"
#include "base/utils/StringIO.h"

TEST_CASE("test/utils/TestPath.cpp", "")
{
    // Test ohkl::fileSeparator
    auto file_sep = ohkl::fileSeparator();
    std::string path = file_sep + "usr" + file_sep + "local" + file_sep + "file.txt";
#ifdef _WIN32
    CHECK(path == "\\usr\\local\\file.txt");
#else
    CHECK(path == "/usr/local/file.txt");
#endif

    // Test ohkl::buildPath for bulding a path from a vector of string
    std::vector<std::string> path_components({file_sep + "usr", "local", "file.txt"});
#ifdef _WIN32
    CHECK(ohkl::buildPath(path_components, "") == path);
#else
    CHECK(ohkl::buildPath(path_components, "") == path);
#endif

    // Test ohkl::splitFileExtension for splitting a filename from its filename and
    // its extension Case where the filename has an extension
    auto ext_splitted_filename = ohkl::splitFileExtension(path);
#ifdef _WIN32
    CHECK(ext_splitted_filename.first == "\\usr\\local\\file");
#else
    CHECK(ext_splitted_filename.first == "/usr/local/file");
#endif
    CHECK(ext_splitted_filename.second == ".txt");

    // Test ohkl::splitFileExtension for splitting a filename from its filename and
    // its extension Case where the filename has no extension
    ext_splitted_filename = ohkl::splitFileExtension(ext_splitted_filename.first);
#ifdef _WIN32
    CHECK(ext_splitted_filename.first == "\\usr\\local\\file");
#else
    CHECK(ext_splitted_filename.first == "/usr/local/file");
#endif
    CHECK(ext_splitted_filename.second.empty());

    // Test ohkl::fileBasename function for getting the basename from a path
    auto basename = ohkl::fileBasename(path);
    CHECK(basename == "file.txt");

    // Test ohkl::fileDirname function for getting the directory name from a path
    auto dirname = ohkl::fileDirname(path);
#ifdef _WIN32
    CHECK(dirname == "\\usr\\local");
#else
    CHECK(dirname == "/usr/local");
#endif
}
