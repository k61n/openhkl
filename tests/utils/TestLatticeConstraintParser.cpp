#define BOOST_TEST_MODULE "Test Lattice Constraint Parser"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>

#include <nsxlib/utils/LatticeConstraintParser.h>

using namespace nsx::Utils;

BOOST_AUTO_TEST_CASE(Test_LatticeConstraintParser)
{
    namespace filesystem = boost::filesystem;

    LatticeConstraintParser<std::string::iterator> parser;

    nsx::Utils::constraints_set constraints;

    std::string s("a=2b");
    BOOST_CHECK(qi::phrase_parse(s.begin(),s.end(),parser,qi::blank,constraints));
}
