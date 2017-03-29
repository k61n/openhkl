#define BOOST_TEST_MODULE "Test Isotope"
#define BOOST_TEST_DYN_LINK

#include <complex>
#include <string>
#include <stdexcept>

#include <boost/test/unit_test.hpp>

#include <nsxlib/chemistry/Isotope.h>
#include <nsxlib/chemistry/IsotopeDatabaseManager.h>
#include <nsxlib/utils/Units.h>

using SX::Chemistry::Isotope;
using SX::Chemistry::IsotopeDatabaseManager;
using SX::Units::fm;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Isotope)
{
    IsotopeDatabaseManager* mgr=IsotopeDatabaseManager::Instance();

    Isotope hf176=mgr->getIsotope("Hf[176]");

    BOOST_CHECK_EQUAL(hf176.getProperty<std::complex<double>>("b_coherent"),std::complex<double>(6.61*1.0e-15,0));

    BOOST_CHECK(hf176.hasProperty("nuclear_spin"));

    BOOST_CHECK(!hf176.hasProperty("xxxxxx"));

    BOOST_CHECK_THROW(hf176.getProperty<int>("xxxxxx"),std::runtime_error);
}
