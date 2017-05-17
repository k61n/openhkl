#define BOOST_TEST_MODULE "Test Lattice Constraint Parser"
#define BOOST_TEST_DYN_LINK

#include <string>

#include <boost/test/unit_test.hpp>

#include <nsxlib/utils/LatticeConstraintParser.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_LatticeConstraintParser)
{
    LatticeConstraintParser<std::string::iterator> parser;

    nsx::constraints_set constraints;

    std::string s("a=2b");
    BOOST_CHECK(qi::phrase_parse(s.begin(),s.end(),parser,qi::blank,constraints));
}
