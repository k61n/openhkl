#include <stdexcept>

#include "core/chemistry/Material.h"
#include "core/utils/NSXTest.h"
#include "core/utils/Units.h"

const double tolerance = 1.0e-9;

NSX_INIT_TEST

int main()
{
    NSX_CHECK_THROW(nsx::Material junk("X"), std::runtime_error);

    nsx::Material methane("CH4");
    auto isotopes = methane.isotopes();

    NSX_CHECK_CLOSE(isotopes.at("C[12]"), 0.989, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("C[13]"), 0.011, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 4.0 * 0.999850, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[2]"), 4.0 * 0.000150, tolerance);
    NSX_CHECK_CLOSE(methane.molarMass(), 0.016042940797287603, tolerance);

    NSX_CHECK_ASSERT(methane.formula().compare("CH4") == 0);

    NSX_CHECK_THROW(methane.setMassDensity(0.0), std::runtime_error);
    NSX_CHECK_THROW(methane.setMassDensity(-10.0), std::runtime_error);
    NSX_CHECK_NO_THROW(methane.setMassDensity(1.235));

    nsx::isotopeContents massFractions = methane.massFractions();
    // Set the molar in g_per_mol
    double methaneMolarMass = nsx::kilo * methane.molarMass();
    NSX_CHECK_CLOSE(massFractions.at("C[12]"), 12.00000000 * 0.98900 / methaneMolarMass, tolerance);
    NSX_CHECK_CLOSE(massFractions.at("C[13]"), 13.00335484 * 0.01100 / methaneMolarMass, tolerance);
    NSX_CHECK_CLOSE(
        massFractions.at("H[1]"), 4.00000000 * 1.007825032 * 0.99985 / methaneMolarMass, tolerance);
    NSX_CHECK_CLOSE(
        massFractions.at("H[2]"), 4.00000000 * 2.014101778 * 0.00015 / methaneMolarMass, tolerance);

    nsx::Material water("H2O");
    isotopes = water.isotopes();
    NSX_CHECK_CLOSE(isotopes.at("O[16]"), 0.99762, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("O[17]"), 0.00038, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("O[18]"), 0.002, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 2.0 * 0.999850, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[2]"), 2.0 * 0.000150, tolerance);
    NSX_CHECK_CLOSE(water.molarMass(), 0.018015256661, tolerance);

    // mass density in kg/m3
    water.setMassDensity(2.3);
    double waterMolarMass = water.molarMass();
    nsx::isotopeContents atomicDensity = water.atomicNumberDensity();
    NSX_CHECK_CLOSE(
        atomicDensity.at("O[16]"), nsx::avogadro * 0.99762 * water.massDensity() / waterMolarMass,
        tolerance);
    NSX_CHECK_CLOSE(
        atomicDensity.at("O[17]"), nsx::avogadro * 0.00038 * water.massDensity() / waterMolarMass,
        tolerance);
    NSX_CHECK_CLOSE(
        atomicDensity.at("O[18]"), nsx::avogadro * 0.002 * water.massDensity() / waterMolarMass,
        tolerance);
    NSX_CHECK_CLOSE(
        atomicDensity.at("H[1]"),
        nsx::avogadro * 2.0 * 0.999850 * water.massDensity() / waterMolarMass, tolerance);
    NSX_CHECK_CLOSE(
        atomicDensity.at("H[2]"),
        nsx::avogadro * 2.0 * 0.000150 * water.massDensity() / waterMolarMass, tolerance);

    double dO16 = atomicDensity.at("O[16]");
    double dO17 = atomicDensity.at("O[17]");
    double dO18 = atomicDensity.at("O[18]");
    double dH1 = atomicDensity.at("H[1]");
    double dH2 = atomicDensity.at("H[2]");
    NSX_CHECK_CLOSE(
        water.muIncoherent(),
        nsx::barn * (dO16 * 0.0 + dO17 * 0.004 + dO18 * 0.0 + dH1 * 80.270 + dH2 * 2.05),
        tolerance);
    NSX_CHECK_CLOSE(
        water.muAbsorption(4.8 * nsx::ang),
        nsx::barn
            * (dO16 * 0.0001 + dO17 * 0.23599999999999999 + dO18 * 0.00016000000000000001
               + dH1 * 0.33260000000000001 + dH2 * 0.00051900000000000004)
            * 4.8 / 1.798,
        tolerance);

    // Check deuterium D shortcut
    nsx::Material deuterium1("H[2]");
    nsx::Material deuterium2("D");

    for (auto p : deuterium1.isotopes()) {
        NSX_CHECK_CLOSE(p.second, deuterium2.isotopes().at(p.first), tolerance);
    }

    // Check Tritum T shortcut
    nsx::Material tritium1("H[3]");
    nsx::Material tritium2("T");

    for (auto p : tritium1.isotopes()) {
        NSX_CHECK_CLOSE(p.second, tritium2.isotopes().at(p.first), tolerance);
    }

    return 0;
}
