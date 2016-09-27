#define BOOST_TEST_MODULE "Test Physical Unit"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

#include "PhysicalUnit.h"

BOOST_AUTO_TEST_CASE(Test_Physical_Unit)
{
	SX::Physics::PhysicalUnit p(1.0,"m*s");

	std::cout<<p.getConversionFactor()<<std::endl;

	auto dimension = p.getDimension();

	std::copy(dimension.begin(), dimension.end(), std::ostream_iterator<int>(std::cout));
	std::cout<<std::endl;

	auto factor=p.convert("km*s");
	std::cout<<factor<<std::endl;
}
