#define BOOST_TEST_MODULE "Test Isotope"
#define BOOST_TEST_DYN_LINK

#include <string>

#include <boost/test/unit_test.hpp>

#include <nsxlib/kernel/Error.h>
#include <nsxlib/chemistry/ChemicalDatabaseManager.h>
#include <nsxlib/chemistry/Isotope.h>
#include <nsxlib/utils/Units.h>

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope)
{
    ChemicalDatabaseManager<Isotope>* mgr=ChemicalDatabaseManager<Isotope>::Instance();
    UnitsManager* um=UnitsManager::Instance();

    // Checks that setting the isotope database to a correct path does not throw
    BOOST_CHECK_NO_THROW(mgr->loadDatabase("./isotopes.xml"));

    // Get an isotope known from the isotope database
    ChemicalDatabaseManager<Isotope>::sptrChemicalObject is=mgr->getChemicalObject("H[1]");

    // Checks some of the property of the isotope
    BOOST_CHECK_CLOSE(is->getProperty<double>("molar_mass"),1.00782504*um->get("g_per_mole"),tolerance);

    mgr->saveDatabase("toto.xml");
}
