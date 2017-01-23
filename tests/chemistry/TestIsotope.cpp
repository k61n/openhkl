#define BOOST_TEST_MODULE "Test Isotope"
#define BOOST_TEST_DYN_LINK

#include <string>

#include <boost/test/unit_test.hpp>

#include "Error.h"
#include "IsotopeManager.h"
#include "Units.h"

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope)
{
    IsotopeManager* mgr=IsotopeManager::Instance();
    UnitsManager* um=UnitsManager::Instance();

    // Checks that setting the isotope database to a wrong path throws
    BOOST_CHECK_THROW(mgr->setDatabasePath("/fsdfs/fsdfsd/blablabla.xml"),SX::Kernel::Error<IsotopeManager>);

    // Checks that setting the isotope database to a correct path does not throw
    BOOST_CHECK_NO_THROW(mgr->setDatabasePath("./isotopes.xml"));

    // Builds an isotope known from the isotope database
    sptrIsotope is=mgr->getIsotope("H[1]");

    // Checks that the isotopes registry has been updated.
    BOOST_CHECK_EQUAL(mgr->getNIsotopesInRegistry(),1);

    // Builds the same isotope and check that the isotopes registry has no been modified because no new isotope was created
    sptrIsotope is1=mgr->getIsotope("H[1]");
    BOOST_CHECK_EQUAL(mgr->getNIsotopesInRegistry(),1);

    // Checks some of the property of the isotope
    BOOST_CHECK_EQUAL(is->getFormalCharge(),0);
    BOOST_CHECK_CLOSE(is->getMolarMass(),1.00782504*um->get("g_per_mole"),tolerance);

    // Takes all the isotopes whose symbol is equal to O
    isotopeSet isset=mgr->getIsotopes<std::string>("symbol","O");
    // Checks that the set contains the 3 known O isotopes registered in the isotopes database (O[16],O[17],O[18])
    BOOST_CHECK_EQUAL(isset.size(),3);
    // Checks that the isotopes registry has now been modified with 3 new entries
    BOOST_CHECK_EQUAL(mgr->getNIsotopesInRegistry(),4);

    // Checks that accessing a given property of a given isotope of the isotopes database is OK and that it does not modify the registry
    BOOST_CHECK_EQUAL(mgr->getProperty<int>("N[14]","n_nucleons"),14);
    BOOST_CHECK_EQUAL(mgr->getNIsotopesInRegistry(),4);

    // Checks that accessing a given property of an isotope but with the wrong type throws
    BOOST_CHECK_THROW(mgr->getProperty<double>("N[14]","name"),SX::Kernel::Error<IsotopeManager>);

    // Checks that accessing a given property of an unknown isotope throws
    BOOST_CHECK_THROW(mgr->getProperty<int>("XXXX","n_nucleons"),SX::Kernel::Error<IsotopeManager>);

    // Checks that accessing an unknown property of the isotopes database throws
    BOOST_CHECK_THROW(mgr->getProperty<int>("N[14]","XXXXX"),SX::Kernel::Error<IsotopeManager>);

    // Cheks that a given known isotope is actually stored in the XML database
    BOOST_CHECK(mgr->isInDatabase("He[4]"));
    // Cheks that a given unknown isotope is actually not stored in the XML database
    BOOST_CHECK(!mgr->isInDatabase("vdvxdvsdv"));

    // IsotopeManager::DestroyInstance();
}
