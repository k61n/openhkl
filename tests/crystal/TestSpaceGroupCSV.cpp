#define BOOST_TEST_MODULE "Test Space group CSV"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <map>
#include <string>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/utils/CSV.h>

int run_test()
{
    std::ifstream csv_file;

    nsx::CSV csv_reader('\t', '#');

    csv_file.open("crystallography.tsv", std::ifstream::in);

    BOOST_CHECK(csv_file.is_open());

    // skip header
    csv_reader.getRow(csv_file);

    while(!csv_file.eof()) {
        std::vector<std::string> row = csv_reader.getRow(csv_file);

        // extra newline at end of file
        if (csv_file.eof() && row.size() < 8)
            continue;

        BOOST_CHECK(row.size() == 8);

        std::cout<<row[0]<<std::endl;

        BOOST_CHECK_NO_THROW(nsx::SpaceGroup sg(row[0]));
    }

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_SpaceGroupCSV)
{
    BOOST_CHECK(run_test() == 0);
}
