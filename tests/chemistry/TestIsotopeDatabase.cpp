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
    nsx::IsotopeDatabaseManager* imgr = nsx::IsotopeDatabaseManager::Instance();
    nsx::UnitsManager* um = nsx::UnitsManager::Instance();

    BOOST_CHECK_EQUAL(imgr->getProperty<std::complex<double>>("Hf[176]","b_coherent"),std::complex<double>(6.61*1.0e-15,0));

    BOOST_CHECK(imgr->hasProperty("Hf[176]","nuclear_spin"));

    BOOST_CHECK(!imgr->hasProperty("Hf[176]","xxxxxx"));

    // Checks some of the property of the isotope
    BOOST_CHECK_CLOSE(imgr->getProperty<double>("H[1]","molar_mass"),1.00782504*um->get("g_per_mole"),tolerance);
    BOOST_CHECK_EQUAL(imgr->getProperty<int>("H[1]","n_protons"),1);
    BOOST_CHECK(imgr->getProperty<bool>("H[1]","stable"));
    BOOST_CHECK(imgr->getProperty<std::string>("H[1]","symbol").compare("H")==0);

    BOOST_CHECK_EQUAL(imgr->getProperty<int>("Ag[107]","n_neutrons"),60);
}
