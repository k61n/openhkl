#define BOOST_TEST_MODULE "Test CSV class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "CSV.h"

using namespace SX::Utils;

int run_test() {
    std::string csv_file =
            "a, b, c, d\n"
            "e, f, g, h\n"
            "1, 2, 3, 4\n"
            "\"as,df\", \"\"\",d,e";

    std::istringstream stream(csv_file);

    SX::Utils::CSV csv;

    std::vector<std::string> row;

    while(!stream.eof()) {
        row = csv.getRow(stream);
        BOOST_CHECK(row.size() == 4);

        for (auto&& entry: row)
            std::cout << entry << ", ";

        std::cout << std::endl;
    }

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_CSV)
{
    BOOST_CHECK(run_test() == 0);
}
