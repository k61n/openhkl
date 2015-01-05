#define BOOST_TEST_MODULE "Test Isotope Manager"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "IsotopeManager.h"
#include "Units.h"

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope_Manager)
{

	UnitsManager* um=UnitsManager::Instance();

	IsotopeManager* im=IsotopeManager::Instance();

	Isotope* is=im->getIsotope("H[1]");

	BOOST_CHECK_EQUAL(is->getFormalCharge(),0);
	BOOST_CHECK_CLOSE(is->getMolarMass(),1.00782504*um->get("uma"),tolerance);
}
