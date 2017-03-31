#define BOOST_TEST_MODULE "Test Material"
#define BOOST_TEST_DYN_LINK

#include <stdexcept>

#include <boost/test/unit_test.hpp>

#include <nsxlib/chemistry/Material.h>
#include <nsxlib/utils/Units.h>
#include <nsxlib/utils/Types.h>

#include <iostream>

const double tolerance=1.0e-9;

using SX::Chemistry::isotopeContents;
using SX::Chemistry::Material;
using SX::Units::ang;
using SX::Units::avogadro;
using SX::Units::barn;
using SX::Units::kilo;

BOOST_AUTO_TEST_CASE(Test_Material)
{
    BOOST_CHECK_THROW(Material junk("X"),std::runtime_error);

    Material methane("CH4");
    auto isotopes=methane.isotopes();

    BOOST_CHECK_CLOSE(isotopes.at("C[12]"),0.989,tolerance);
    BOOST_CHECK_CLOSE(isotopes.at("C[13]"),0.011,tolerance);
    BOOST_CHECK_CLOSE(isotopes.at("H[1]"),4.0*0.999850,tolerance);
    BOOST_CHECK_CLOSE(isotopes.at("H[2]"),4.0*0.000150,tolerance);
    BOOST_CHECK_CLOSE(methane.molarMass(),0.016042940797287603,tolerance);

    BOOST_CHECK(methane.formula().compare("CH4")==0);

    BOOST_CHECK_THROW(methane.setMassDensity(0.0),std::runtime_error);
    BOOST_CHECK_THROW(methane.setMassDensity(-10.0),std::runtime_error);
    BOOST_CHECK_NO_THROW(methane.setMassDensity(1.235));

    isotopeContents massFractions=methane.massFractions();
    // Set the molar in g_per_mol
    double methaneMolarMass = kilo*methane.molarMass();
    BOOST_CHECK_CLOSE(massFractions.at("C[12]"), 12.00000000*0.98900/methaneMolarMass            , tolerance);
    BOOST_CHECK_CLOSE(massFractions.at("C[13]"), 13.00335484*0.01100/methaneMolarMass            , tolerance);
    BOOST_CHECK_CLOSE(massFractions.at("H[1]") ,  4.00000000*1.007825032*0.99985/methaneMolarMass, tolerance);
    BOOST_CHECK_CLOSE(massFractions.at("H[2]") ,  4.00000000*2.014101778*0.00015/methaneMolarMass, tolerance);

    Material water("H2O");
    isotopes = water.isotopes();
    BOOST_CHECK_CLOSE(isotopes.at("O[16]"),        0.99762, tolerance);
    BOOST_CHECK_CLOSE(isotopes.at("O[17]"),        0.00038, tolerance);
    BOOST_CHECK_CLOSE(isotopes.at("O[18]"),          0.002, tolerance);
    BOOST_CHECK_CLOSE(isotopes.at("H[1]") ,   2.0*0.999850, tolerance);
    BOOST_CHECK_CLOSE(isotopes.at("H[2]") ,   2.0*0.000150, tolerance);
    BOOST_CHECK_CLOSE(water.molarMass()   , 0.018015256661, tolerance);

    // mass density in kg/m3
    water.setMassDensity(2.3);
    double waterMolarMass = water.molarMass();
    isotopeContents atomicDensity=water.atomicNumberDensity();
    BOOST_CHECK_CLOSE(atomicDensity.at("O[16]"),      avogadro*0.99762*water.massDensity()/waterMolarMass, tolerance);
    BOOST_CHECK_CLOSE(atomicDensity.at("O[17]"),      avogadro*0.00038*water.massDensity()/waterMolarMass, tolerance);
    BOOST_CHECK_CLOSE(atomicDensity.at("O[18]"),        avogadro*0.002*water.massDensity()/waterMolarMass, tolerance);
    BOOST_CHECK_CLOSE(atomicDensity.at("H[1]") , avogadro*2.0*0.999850*water.massDensity()/waterMolarMass, tolerance);
    BOOST_CHECK_CLOSE(atomicDensity.at("H[2]") , avogadro*2.0*0.000150*water.massDensity()/waterMolarMass, tolerance);

    double dO16=atomicDensity.at("O[16]");
    double dO17=atomicDensity.at("O[17]");
    double dO18=atomicDensity.at("O[18]");
    double dH1=atomicDensity.at("H[1]");
    double dH2=atomicDensity.at("H[2]");
    BOOST_CHECK_CLOSE(water.muIncoherent(), barn*(dO16*0.0+dO17*0.004+dO18*0.0+dH1*80.270+dH2*2.05), tolerance);
    BOOST_CHECK_CLOSE(water.muAbsorption(4.8*ang), barn*(dO16*0.0001+
                                                         dO17*0.23599999999999999+
                                                         dO18*0.00016000000000000001+
                                                         dH1*0.33260000000000001+
                                                         dH2*0.00051900000000000004)*4.8/1.798, tolerance);

    // Check deuterium D shortcut
    Material deuterium1("H[2]");
    Material deuterium2("D");

    for (auto p : deuterium1.isotopes()) {
        BOOST_CHECK_CLOSE(p.second,deuterium2.isotopes().at(p.first),tolerance);
    }

    // Check Tritum T shortcut
    Material tritium1("H[3]");
    Material tritium2("T");

    for (auto p : tritium1.isotopes()) {
        BOOST_CHECK_CLOSE(p.second,tritium2.isotopes().at(p.first),tolerance);
    }

}
