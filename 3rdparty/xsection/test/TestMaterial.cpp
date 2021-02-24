#include "catch.hpp"

#include "Material.h"
#include "Units.h"

#include <stdexcept>

const double tolerance = 1.0e-9;

TEST_CASE("test/chemistry/TestMaterial.cpp", "")
{

    CHECK_THROWS_AS(xsection::Material("X"), std::runtime_error);

    xsection::Material methane("CH4");
    auto isotopes = methane.isotopes();

    CHECK(isotopes.at("C[12]") == Approx(0.989).epsilon(tolerance));
    CHECK(isotopes.at("C[13]") == Approx(0.011).epsilon(tolerance));
    CHECK(isotopes.at("H[1]") == Approx(4.0 * 0.999850).epsilon(tolerance));
    CHECK(isotopes.at("H[2]") == Approx(4.0 * 0.000150).epsilon(tolerance));
    CHECK(methane.molarMass() == Approx(0.016042940797287603).epsilon(tolerance));

    CHECK(methane.formula().compare("CH4") == 0);

    CHECK_THROWS_AS(methane.setMassDensity(0.0), std::runtime_error);
    CHECK_THROWS_AS(methane.setMassDensity(-10.0), std::runtime_error);
    CHECK_NOTHROW(methane.setMassDensity(1.235));

    xsection::isotopeContents massFractions = methane.massFractions();
    // Set the molar in g_per_mol
    double methaneMolarMass = xsection::kilo * methane.molarMass();
    CHECK(
        massFractions.at("C[12]")
        == Approx(12.00000000 * 0.98900 / methaneMolarMass).epsilon(tolerance));
    CHECK(
        massFractions.at("C[13]")
        == Approx(13.00335484 * 0.01100 / methaneMolarMass).epsilon(tolerance));
    CHECK(
        massFractions.at("H[1]")
        == Approx(4.00000000 * 1.007825032 * 0.99985 / methaneMolarMass).epsilon(tolerance));
    CHECK(
        massFractions.at("H[2]")
        == Approx(4.00000000 * 2.014101778 * 0.00015 / methaneMolarMass).epsilon(tolerance));

    xsection::Material water("H2O");
    isotopes = water.isotopes();
    CHECK(isotopes.at("O[16]") == Approx(0.99762).epsilon(tolerance));
    CHECK(isotopes.at("O[17]") == Approx(0.00038).epsilon(tolerance));
    CHECK(isotopes.at("O[18]") == Approx(0.002).epsilon(tolerance));
    CHECK(isotopes.at("H[1]") == Approx(2.0 * 0.999850).epsilon(tolerance));
    CHECK(isotopes.at("H[2]") == Approx(2.0 * 0.000150).epsilon(tolerance));
    CHECK(water.molarMass() == Approx(0.018015256661).epsilon(tolerance));

    // mass density in kg/m3
    water.setMassDensity(2.3);
    double waterMolarMass = water.molarMass();
    xsection::isotopeContents atomicDensity = water.atomicNumberDensity();
    CHECK(
        atomicDensity.at("O[16]")
        == Approx(xsection::avogadro * 0.99762 * water.massDensity() / waterMolarMass)
               .epsilon(tolerance));
    CHECK(
        atomicDensity.at("O[17]")
        == Approx(xsection::avogadro * 0.00038 * water.massDensity() / waterMolarMass)
               .epsilon(tolerance));
    CHECK(
        atomicDensity.at("O[18]")
        == Approx(xsection::avogadro * 0.002 * water.massDensity() / waterMolarMass).epsilon(tolerance));
    CHECK(
        atomicDensity.at("H[1]")
        == Approx(xsection::avogadro * 2.0 * 0.999850 * water.massDensity() / waterMolarMass)
               .epsilon(tolerance));
    CHECK(
        atomicDensity.at("H[2]")
        == Approx(xsection::avogadro * 2.0 * 0.000150 * water.massDensity() / waterMolarMass)
               .epsilon(tolerance));

    double dO16 = atomicDensity.at("O[16]");
    double dO17 = atomicDensity.at("O[17]");
    double dO18 = atomicDensity.at("O[18]");
    double dH1 = atomicDensity.at("H[1]");
    double dH2 = atomicDensity.at("H[2]");
    CHECK(
        water.muIncoherent()
        == Approx(xsection::barn * (dO16 * 0.0 + dO17 * 0.004 + dO18 * 0.0 + dH1 * 80.270 + dH2 * 2.05))
               .epsilon(tolerance));
    CHECK(
        water.muAbsorption(4.8 * xsection::ang)
        == Approx(
               xsection::barn
               * (dO16 * 0.0001 + dO17 * 0.23599999999999999 + dO18 * 0.00016000000000000001
                  + dH1 * 0.33260000000000001 + dH2 * 0.00051900000000000004)
               * 4.8 / 1.798)
               .epsilon(tolerance));

    // Check deuterium D shortcut
    xsection::Material deuterium1("H[2]");
    xsection::Material deuterium2("D");

    for (auto p : deuterium1.isotopes())
        CHECK(p.second == Approx(deuterium2.isotopes().at(p.first)).epsilon(tolerance));

    // Check Tritum T shortcut
    xsection::Material tritium1("H[3]");
    xsection::Material tritium2("T");

    for (auto p : tritium1.isotopes())
        CHECK(p.second == Approx(tritium2.isotopes().at(p.first)).epsilon(tolerance));
}
