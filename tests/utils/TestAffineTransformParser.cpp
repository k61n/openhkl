#define BOOST_TEST_MODULE "Test Affine Transform Parser"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>
#include <nsxlib/utils/AffineTransformParser.h>
#include <Eigen/Dense>
#include <boost/spirit/include/classic.hpp>

#include <boost/algorithm/string.hpp>
#include <vector>

using namespace nsx;
using namespace Eigen;
const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_AffineTransformParser)
{

    std::string s("x-z+1/3,y-2x,z+3.4");

    Eigen::Transform<double,3,Eigen::Affine> m;
    AffineTransformParser<std::string::iterator> parser1;
    bool test=qi::phrase_parse(s.begin(),s.end(),parser1,qi::blank, m);

    // Validate parsing
    BOOST_CHECK(test);
    // Compare
    BOOST_CHECK_CLOSE(m(0,0),1,tolerance);
    BOOST_CHECK_CLOSE(m(0,1),0,tolerance);
    BOOST_CHECK_CLOSE(m(0,2),-1,tolerance);
    BOOST_CHECK_CLOSE(m(0,3),1.0/3.0,tolerance);
    BOOST_CHECK_CLOSE(m(1,0),-2,tolerance);
    BOOST_CHECK_CLOSE(m(1,1),1,tolerance);
    BOOST_CHECK_CLOSE(m(1,2),0,tolerance);
    BOOST_CHECK_CLOSE(m(1,3),0,tolerance);
    BOOST_CHECK_CLOSE(m(2,0),0,tolerance);
    BOOST_CHECK_CLOSE(m(2,1),0,tolerance);
    BOOST_CHECK_CLOSE(m(2,2),1,tolerance);
    BOOST_CHECK_CLOSE(m(2,3),3.4,tolerance);
    BOOST_CHECK_CLOSE(m(3,0),0,tolerance);
    BOOST_CHECK_CLOSE(m(3,1),0,tolerance);
    BOOST_CHECK_CLOSE(m(3,2),0,tolerance);
    BOOST_CHECK_CLOSE(m(3,3),1,tolerance);

    std::string generators(" x+1/2,y+1/2,z; -x+1/2,-y,z; -x,y,-z+1/2; -x,-y,-z");
    std::vector<std::string> strs;
    boost::split(strs, generators, boost::is_any_of(";"));

    std::for_each(strs.begin(),strs.end(),[](std::string& a){std::cout << a << std::endl;});
    for (auto& s: strs)
    {
        Eigen::Transform<double,3,Eigen::Affine> m;
        if (qi::phrase_parse(s.begin(),s.end(),parser1,qi::blank, m))
        {
            std::cout << m.matrix() << std::endl;
        }
    }


}
