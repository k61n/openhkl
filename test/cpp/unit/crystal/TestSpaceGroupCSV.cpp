//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/crystal/TestSpaceGroupCSV.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/utils/CSV.h"
#include "tables/crystal/SpaceGroup.h"

#include <fstream>
#include <string>
#include <iostream>

TEST_CASE("test/crystal/TestSpaceGroupCSV.cpp", "")
{
    std::ifstream csv_file;
    csv_file.open("crystallography.tsv", std::ifstream::in);
    if (!csv_file.is_open()) {
        std::cerr << "failed to open crystallography.tsv\n";
        CHECK(false);
        return;
    }

    ohkl::CSV csv_reader('\t', '#');

    while (!csv_file.eof()) {
        std::vector<std::string> row = csv_reader.getRow(csv_file);

        // extra newline at end of file
        if (csv_file.eof() && row.size() < 8)
            continue;

        CHECK(row.size() == 8);

        std::cout << row[0] << std::endl;

        CHECK_NOTHROW(ohkl::SpaceGroup{row[0]});
    }
}
