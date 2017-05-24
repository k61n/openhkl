#define BOOST_TEST_MODULE "Test Isotope Database"
#define BOOST_TEST_DYN_LINK

#include <string>

#include <stdexcept>

#include <boost/test/unit_test.hpp>

#include <nsxlib/chemistry/IsotopeDatabaseManager.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope_Database)
{
    nsx::IsotopeDatabaseManager* mgr = nsx::IsotopeDatabaseManager::Instance();
    nsx::UnitsManager* um = nsx::UnitsManager::Instance();

    auto hf176=mgr->getIsotope("Hf[176]");

    BOOST_CHECK_EQUAL(hf176.getProperty<std::complex<double>>("b_coherent"),std::complex<double>(6.61*1.0e-15,0));

    BOOST_CHECK(hf176.hasProperty("nuclear_spin"));

    BOOST_CHECK(!hf176.hasProperty("xxxxxx"));

    BOOST_CHECK_THROW(hf176.getProperty<int>("xxxxxx"),std::runtime_error);

    // Get an isotope known from the isotope database
    auto h1=mgr->getIsotope("H[1]");

    // Checks some of the property of the isotope
    BOOST_CHECK_CLOSE(h1.getProperty<double>("molar_mass"),1.00782504*um->get("g_per_mole"),tolerance);
    BOOST_CHECK_EQUAL(h1.getProperty<int>("n_protons"),1);
    BOOST_CHECK(h1.getProperty<bool>("stable"));
    BOOST_CHECK(h1.getProperty<std::string>("symbol").compare("H")==0);

    auto database = mgr->database();

    auto ag107 = database.at("Ag[107]");
    BOOST_CHECK_EQUAL(ag107.getProperty<int>("n_neutrons"),60);
}
