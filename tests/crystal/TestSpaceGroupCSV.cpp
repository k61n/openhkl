#define BOOST_TEST_MODULE "Test Space group CSV"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <map>
#include <string>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/SpaceGroupSymbols.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/utils/CSV.h>

int run_test()
{
    std::ifstream csv_file;
    nsx::SpaceGroupSymbols* table = nsx::SpaceGroupSymbols::Instance();
    int num_rows = 0;

    std::map<std::string, int> counts;

    std::vector<std::string> symbols = table->getAllSymbols();

    for (auto&& symbol: symbols)
        counts[symbol] = 0;

    nsx::CSV csv_reader('\t', '#');
    std::vector<std::string> unrecognized_symbols;

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

        std::string sg, sgHall;

        sg = table->getReducedSymbol(row[0]);
        sgHall = row[1];

        std::string generators;

        bool found = table->getGenerators(sg, generators);

        if (!found) {
            found = table->getGenerators(sgHall, generators);
        }

        // still not found, add to list
        if (!found) {
            if (find(unrecognized_symbols.begin(), unrecognized_symbols.end(), sg) == unrecognized_symbols.end())
                unrecognized_symbols.push_back(sg);
        }

        if (found) {
            std::string full_name = table->getFullSymbol(sg);
            ++counts[full_name];
        }

        ++num_rows;
    }

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_SpaceGroupCSV)
{
    BOOST_CHECK(run_test() == 0);
}
