#define BOOST_TEST_MODULE "Test Physical Unit"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <exception>
#include <iostream>
#include <string>

#include "PhysicalUnit.h"

const double tolerance=1.0e-9;

BOOST_AUTO_TEST_CASE(Test_Physical_Unit)
{
	// Check that a bad input unit actually throws
	BOOST_CHECK_THROW(SX::Physics::PhysicalUnit p(1.0,"xxxxxx"),std::runtime_error);

	SX::Physics::PhysicalUnit p(1.0,"m/s");

	// Check that an invalid conversion throws
	BOOST_CHECK_THROW(p.convert("km*K"),std::runtime_error);

	// Check a valid conversion
    BOOST_CHECK_CLOSE(p.convert("km/s"), 1.0e-3, tolerance);

    // Change the value
    p.setValue(5.0);

    // Check again a valid conversion
    BOOST_CHECK_CLOSE(p.convert("km/s"), 5.0e-3, tolerance);

}
