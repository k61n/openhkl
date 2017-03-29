#define BOOST_TEST_MODULE "Test Isotope Database"
#define BOOST_TEST_DYN_LINK

#include <string>
#include <stdexcept>

#include <boost/test/unit_test.hpp>

#include <nsxlib/chemistry/Isotope.h>
#include <nsxlib/chemistry/IsotopeDatabaseManager.h>
#include <nsxlib/utils/Units.h>

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope_Database)
{
    IsotopeDatabaseManager* mgr=IsotopeDatabaseManager::Instance();
    UnitsManager* um=UnitsManager::Instance();

    BOOST_CHECK_THROW(mgr->loadDatabase("./xxxxxx.xml"),std::runtime_error);

    // Checks that setting the isotope database to a correct path does not throw
    BOOST_CHECK_NO_THROW(mgr->loadDatabase("./isotopes.xml"));

    // Get an isotope known from the isotope database
    Isotope h1=mgr->getIsotope("H[1]");

    // Checks some of the property of the isotope
    BOOST_CHECK_CLOSE(h1.getProperty<double>("molar_mass"),1.00782504*um->get("g_per_mole"),tolerance);
    BOOST_CHECK_EQUAL(h1.getProperty<int>("n_protons"),1);
    BOOST_CHECK(h1.getProperty<bool>("stable"));
    BOOST_CHECK(h1.getProperty<std::string>("symbol").compare("H")==0);

    auto database = mgr->database();

    Isotope ag107 = database.at("Ag[107]");
    BOOST_CHECK_EQUAL(ag107.getProperty<int>("n_neutrons"),60);

    mgr->saveDatabase("toto.xml");
}
