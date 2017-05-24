#define BOOST_TEST_MODULE "Test CSV class"
#define BOOST_TEST_DYN_LINK

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <nsxlib/utils/CSV.h>

int run_test() {
    std::string csv_file =
        //    ",,,\n"
        //    "a, b, c, d\n"
        //    "e, f, g, h\n"
        //    "1, 2, 3, 4\n"
            "\"\", \"\"\"\"\", \"as,df\", \" \"\"\" \"";

    std::istringstream stream(csv_file);
    nsx::CSV csv;
    std::vector<std::string> row;

    while(!stream.eof()) {
        row = csv.getRow(stream);
        BOOST_CHECK(row.size() == 4);
    }
    return 0;
}

BOOST_AUTO_TEST_CASE(Test_CSV)
{
    BOOST_CHECK(run_test() == 0);
}
