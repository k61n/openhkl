#include "test/cpp/catch.hpp"

#include "core/utils/CSV.h"

#include <sstream>
#include <string>
#include <vector>

// Test the CSV parser, which in turn seems to be needed for testing only.

TEST_CASE("test/utils/TestCSV-default_separators", "")
{

    std::string csv_file =
        ",,,\n"
        "a, b, c, d\n"
        "\"\", \"\"\"\"\", \"as,df\", \" \"\"\" \"\n" // test whether characters " and , are escaped
        "e, f, g, h";

    std::istringstream stream(csv_file);
    nsx::CSV csv;
    std::vector<std::string> row;

    while (!stream.eof()) {
        row = csv.getRow(stream);
        CHECK(row.size() == 4);
    }
    std::cout << "Test 1 ok\n\n";
}

TEST_CASE("test/utils/TestCSV-user_defined_separators", "")
{

    std::string csv_file = "a\tb\tc\td\n"
                           "a\tb\tc\t'd\te'\n"
                           "a\tb\tc\td'\t'f";

    std::istringstream stream(csv_file);
    nsx::CSV csv('\t', '\'');
    std::vector<std::string> row;

    while (!stream.eof()) {
        std::cout << "New line:\n";
        row = csv.getRow(stream);
        CHECK(row.size() == 4);
    }
    std::cout << "Test 2 ok\n\n";
}
