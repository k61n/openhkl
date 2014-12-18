#define BOOST_TEST_MODULE "Test Materials"
#define BOOST_TEST_DYN_LINK

#include <string>

#include <boost/test/unit_test.hpp>

#include "Materials.h"

const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_Materials)
{

	SX::Crystal::Materials mat;
	mat.setDensity(-1.0);
}
