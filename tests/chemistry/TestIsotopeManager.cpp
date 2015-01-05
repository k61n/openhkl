#define BOOST_TEST_MODULE "Test Isotope Manager"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "IsotopeManager.h"

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_Isotope_Manager)
{

	IsotopeManager* im=IsotopeManager::Instance();

	Isotope* is=im->getIsotope("H[1]");





}
