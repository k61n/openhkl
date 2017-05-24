#define BOOST_TEST_MODULE "Test Space group symbols"
#define BOOST_TEST_DYN_LINK

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/SpaceGroupSymbols.h>
#include <nsxlib/crystal/SpaceGroup.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_SpaceGroupSymbols)
{
    nsx::SpaceGroupSymbols* spaceGroupSymbols = nsx::SpaceGroupSymbols::Instance();
    std::vector<std::string> symbols = spaceGroupSymbols->getAllSymbols();

    BOOST_CHECK_EQUAL(symbols.size(), 230);

    auto table=SpaceGroupSymbols::Instance();
    std::string generators;
    bool test=table->getGenerators("P n m a",generators);

    BOOST_CHECK(test);
    // Check that string generator strings are OK s
    BOOST_CHECK(generators.compare(" -x+1/2,-y,z+1/2; -x,y+1/2,-z; -x,-y,-z")==0);


    for (auto&& symbol: symbols) {
        try {

            std::string reduced = table->getReducedSymbol(symbol);

            BOOST_CHECK(symbol == table->getFullSymbol(symbol));
            BOOST_CHECK(table->getFullSymbol(reduced) == symbol);

            SpaceGroup grp(symbol);
        }
        catch(std::exception& e) {
            BOOST_ERROR("Caught exception");
        }
    }
}
