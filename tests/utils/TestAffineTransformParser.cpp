#define BOOST_TEST_MODULE "Test Affine Transform Parser"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>
#include "AffineTransformParser.h"
#include <Eigen/Dense>
#include <boost/spirit/include/classic.hpp>

using namespace SX::Utils;
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

}
