#include "test/catch.hpp"

#include <fstream>
#include <map>
#include <string>

#include "core/utils/CSV.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"

TEST_CASE("test/crystal/TestSpaceGroupCSV.cpp", "") {

    std::ifstream csv_file;

    nsx::CSV csv_reader('\t', '#');

    csv_file.open("crystallography.tsv", std::ifstream::in);

    CHECK(csv_file.is_open());

    // skip header
    csv_reader.getRow(csv_file);

    while (!csv_file.eof()) {
        std::vector<std::string> row = csv_reader.getRow(csv_file);

        // extra newline at end of file
        if (csv_file.eof() && row.size() < 8)
            continue;

        CHECK(row.size() == 8);

        std::cout << row[0] << std::endl;

        CHECK_NOTHROW(nsx::SpaceGroup{row[0]});
    }
}
