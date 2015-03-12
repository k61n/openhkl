#define BOOST_TEST_MODULE "Test Element"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "Element.h"
#include "ElementManager.h"
#include "Error.h"
#include "IsotopeManager.h"
#include "Units.h"

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1.0e-6;

BOOST_AUTO_TEST_CASE(Test_Element)
{
	ElementManager* emgr=ElementManager::Instance();
	IsotopeManager* imgr=IsotopeManager::Instance();
	UnitsManager* um=UnitsManager::Instance();

	// Checks that setting the elements database to a wrong path throws
	BOOST_CHECK_THROW(emgr->setDatabasePath("/fsdfs/fsdfsd/blablabla.xml"),SX::Kernel::Error<ElementManager>);

	// Checks that setting the elements database to a correct path does not throw
	BOOST_CHECK_NO_THROW(emgr->setDatabasePath("./elements.xml"));

	// Builds the natural hydrogen Element directly from the isotopes registry/database
	sptrElement hydrogen=emgr->getElement("H");
	// Checks that the number of protons is OK
	BOOST_CHECK_EQUAL(hydrogen->getNProtons(),1);
	// Checks that the number of neutrons is OK
	BOOST_CHECK_CLOSE(hydrogen->getNNeutrons(),(1*0.00015),tolerance);
	// Checks that the molar mass is OK
	BOOST_CHECK_CLOSE(hydrogen->getMolarMass(),(1.007825032*0.99985+2.014101778*0.00015)*um->get("uma"),tolerance);

	// Checks that the number of natural isotopes of this element is OK (H[1],H[2],H[3])
	BOOST_CHECK_EQUAL(hydrogen->getNIsotopes(),3);

	// Builds an empty element and fills it will isotopes
	sptrElement uranium=emgr->getElement("natU");
	uranium->addIsotope("U[235]");
	uranium->addIsotope("U[238]");
	// Checks that adding an incompatible isotope throws
	BOOST_CHECK_THROW(uranium->addIsotope("C[12]"),SX::Kernel::Error<Element>);
	// Checks that getting the molar mass of an incomplete (sum of abundances != 1 within 1.0-6 tolerance) elements throws
	BOOST_CHECK_THROW(uranium->getMolarMass(),SX::Kernel::Error<Element>);

	// Checks that a registered elemetns could be retrieved properly
	BOOST_CHECK_EQUAL(uranium,emgr->getElement("natU"));


	// Gets an  oxygen Element from the elements XML database built from user-defined abundances
	sptrElement dbOxygen=emgr->getElement("db_oxygen");
	// Checks that it has the correct number of isotopes
	BOOST_CHECK_EQUAL(dbOxygen->getNIsotopes(),3);

	// Gets an hydrogen Element from the elements XML database built from its natural isotopes
	sptrElement dbHydrogen=emgr->getElement("db_hydrogen");
	// Checks that it has the correct number of isotopes
	BOOST_CHECK_EQUAL(dbHydrogen->getNIsotopes(),3);
	// Checks that its mass is the same than the one built previously built from the isotopes registry/database
	BOOST_CHECK_CLOSE(dbHydrogen->getMolarMass(),hydrogen->getMolarMass(),tolerance);

	emgr->saveRegistry("elements_new.xml");

	ElementManager::DestroyInstance();
	IsotopeManager::DestroyInstance();
}
