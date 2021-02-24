#include "IsotopeDatabaseManager.h"
#include "Units.h"
#include "catch.hpp"

const double tolerance = 1e-6;

TEST_CASE("test/chemistry/TestIsotopeDatabase.cpp", "")
{

    xsection::IsotopeDatabaseManager* imgr = xsection::IsotopeDatabaseManager::instance();
    xsection::UnitsManager* um = xsection::UnitsManager::instance();

    CHECK(
        imgr->property<std::complex<double>>("Hf[176]", "b_coherent")
        == std::complex<double>(6.61 * 1.0e-15, 0));

    CHECK(imgr->hasProperty("Hf[176]", "nuclear_spin"));

    CHECK(!imgr->hasProperty("Hf[176]", "xxxxxx"));

    // Checks some of the property of the isotope
    CHECK(
        imgr->property<double>("H[1]", "molar_mass")
        == Approx(1.00782504 * um->get("g_per_mole")).epsilon(tolerance));
    CHECK(imgr->property<int>("H[1]", "n_protons") == 1);
    CHECK(imgr->property<bool>("H[1]", "stable"));
    CHECK(imgr->property<std::string>("H[1]", "symbol").compare("H") == 0);

    CHECK(imgr->property<int>("Ag[107]", "n_neutrons") == 60);
}
