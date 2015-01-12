#define BOOST_TEST_MODULE "Test Isotope"
#define BOOST_TEST_DYN_LINK

#include <string>

#include <boost/test/unit_test.hpp>

#include "Error.h"
#include "Isotope.h"
#include "Units.h"

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope)
{
	// Check that getting an unknown isotope throws
	BOOST_CHECK_THROW(Isotope::buildFromDatabase("XXX"),SX::Kernel::Error<Isotope>)

	// Build an isotope known from the isotopes database
	Isotope* is=Isotope::buildFromDatabase("H[1]");

	// Check that the isotopes registry has been updated.
	BOOST_CHECK_EQUAL(Isotope::getNRegisteredIsotopes(),1);

	// Build the same isotope
	Isotope* is1=Isotope::buildFromDatabase("H[1]");

	// Check that the isotopes registry has no been modified because no new isotope was created
	BOOST_CHECK_EQUAL(Isotope::getNRegisteredIsotopes(),1);

	// Checks some of the property of the isotope
	UnitsManager* um=UnitsManager::Instance();
	BOOST_CHECK_EQUAL(is->getFormalCharge(),0);
	BOOST_CHECK_CLOSE(is->getMolarMass(),1.00782504*um->get("uma"),tolerance);

	// Takes all the isotopes whose symbol is equal to O
	isotopeSet isset=Isotope::getIsotopes<std::string>("symbol","O");
	// Check that the set contains the 3 known is O isotopes registered in the isotopes database (O[16],O[17],O[18])
	BOOST_CHECK_EQUAL(isset.size(),3);

	// Check that the isotopes registry has now been modified with 3 new entries
	BOOST_CHECK_EQUAL(Isotope::getNRegisteredIsotopes(),4);

	// Check that accessing a given property of a given isotope of the isotopes database is OK
	BOOST_CHECK_EQUAL(Isotope::getProperty<int>("N[14]","n_nucleons"),14);

	// Check that accessing a given property of an unknown isotope throws
	BOOST_CHECK_THROW(Isotope::getProperty<int>("XXXX","n_nucleons"),SX::Kernel::Error<Isotope>);

	// Check that accessing an unknown property of the isotopes database throws
	BOOST_CHECK_THROW(Isotope::getProperty<int>("N[14]","XXXXX"),SX::Kernel::Error<Isotope>);

}
