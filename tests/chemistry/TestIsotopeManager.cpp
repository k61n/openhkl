#define BOOST_TEST_MODULE "Test Isotope Manager"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "Error.h"
#include "IsotopeManager.h"
#include "Units.h"

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope_Manager)
{
	IsotopeManager* im=IsotopeManager::Instance();

	// Check that get an unknown element throws
	BOOST_CHECK_THROW(im->getIsotope("XXX"),SX::Kernel::Error<IsotopeManager>)

	Isotope* is2=im->getIsotope("H[1]");
	UnitsManager* um=UnitsManager::Instance();
	BOOST_CHECK_EQUAL(is2->getFormalCharge(),0);
	BOOST_CHECK_CLOSE(is2->getMolarMass(),1.00782504*um->get("uma"),tolerance);
}
