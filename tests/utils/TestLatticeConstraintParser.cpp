#define BOOST_TEST_MODULE "Test Lattice Constraint Parser"
#define BOOST_TEST_DYN_LINK

#include <string>

#include <boost/test/unit_test.hpp>

#include <nsxlib/utils/UtilsTypes.h>
#include <nsxlib/utils/LatticeConstraintParser.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_LatticeConstraintParser)
{
    LatticeConstraintParser<std::string::iterator> parser;

    nsx::ConstraintSet constraints;

    std::string s("a=2b");
    BOOST_CHECK(boost::spirit::qi::phrase_parse(s.begin(),s.end(),parser,boost::spirit::qi::blank,constraints));
}
