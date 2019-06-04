#include "test/cpp/catch.hpp"

#include <core/utils/CSV.h>
#include <core/crystal/SpaceGroup.h>

#include <fstream>
#include <string>

TEST_CASE("test/crystal/TestSpaceGroupCSV.cpp", "") {

    std::ifstream csv_file;
    csv_file.open("crystallography.tsv", std::ifstream::in);
    if (!csv_file.is_open()) {
        std::cerr << "failed to open crystallography.tsv\n";
        CHECK(false);
        return;
    }

    nsx::CSV csv_reader('\t', '#');

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
