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
    // Test nsx::fileSeparator
    auto file_sep = nsx::fileSeparator();
    std::string path = file_sep + "usr" + file_sep + "local" + file_sep + "file.txt";
#ifdef _WIN32
    CHECK(path == "\\usr\\local\\file.txt");
#else
    CHECK(path == "/usr/local/file.txt");
#endif

    // Test nsx::buildPath for bulding a path from a vector of string
    std::vector<std::string> path_components({file_sep + "usr", "local", "file.txt"});
#ifdef _WIN32
    CHECK(nsx::buildPath(path_components, "") == path);
#else
    CHECK(nsx::buildPath(path_components, "") == path);
#endif

    // Test nsx::splitFileExtension for splitting a filename from its filename and
    // its extension Case where the filename has an extension
    auto ext_splitted_filename = nsx::splitFileExtension(path);
#ifdef _WIN32
    CHECK(ext_splitted_filename.first == "\\usr\\local\\file");
#else
    CHECK(ext_splitted_filename.first == "/usr/local/file");
#endif
    CHECK(ext_splitted_filename.second == ".txt");

    // Test nsx::splitFileExtension for splitting a filename from its filename and
    // its extension Case where the filename has no extension
    ext_splitted_filename = nsx::splitFileExtension(ext_splitted_filename.first);
#ifdef _WIN32
    CHECK(ext_splitted_filename.first == "\\usr\\local\\file");
#else
    CHECK(ext_splitted_filename.first == "/usr/local/file");
#endif
    CHECK(ext_splitted_filename.second.empty());

    // Test nsx::fileBasename function for getting the basename from a path
    auto basename = nsx::fileBasename(path);
    CHECK(basename == "file.txt");

    // Test nsx::fileDirname function for getting the directory name from a path
    auto dirname = nsx::fileDirname(path);
#ifdef _WIN32
    CHECK(dirname == "\\usr\\local");
#else
    CHECK(dirname == "/usr/local");
#endif
}
