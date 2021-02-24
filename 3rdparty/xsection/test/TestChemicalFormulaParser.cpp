#include "catch.hpp"
#define NSX_SPIRIT_USE_PHOENIX_V3

#include <stdexcept>
#include <string>

#include "ChemicalFormulaParser.h"

// This had to be included here to avoid lengthy compile error with boost::spirit
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

const double tolerance = 1.0e-9;

TEST_CASE("ChemicalFormulaParser", "")
{
    using boost::spirit::qi::blank;
    using boost::spirit::qi::phrase_parse;

    xsection::isotopeContents isotopes;

    xsection::ChemicalFormulaParser<std::string::iterator> parser;

    // Check pure isotopes

    std::string material;

    material = "H[1]";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("H[1]") == Approx(1.00).epsilon(tolerance));

    isotopes.clear();
    material = "U[235]";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("U[235]") == Approx(1.00).epsilon(tolerance));

    isotopes.clear();
    material = "X";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    isotopes.clear();
    material = "U[420]";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check natural elements

    isotopes.clear();
    material = "Li";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("Li[6]") == Approx(0.075).epsilon(tolerance));
    CHECK(isotopes.at("Li[7]") == Approx(0.925).epsilon(tolerance));

    isotopes.clear();
    material = "Br";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("Br[79]") == Approx(0.5069).epsilon(tolerance));
    CHECK(isotopes.at("Br[81]") == Approx(0.4931).epsilon(tolerance));

    isotopes.clear();
    material = "Z";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check isotope mixtures

    isotopes.clear();
    material = "C2H{H[1](0.22),H[2](0.78)}9";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("C[12]") == Approx(2.0 * 0.98900).epsilon(tolerance));
    CHECK(isotopes.at("C[13]") == Approx(2.0 * 0.01100).epsilon(tolerance));
    CHECK(isotopes.at("H[1]") == Approx(9.0 * 0.22).epsilon(tolerance));
    CHECK(isotopes.at("H[2]") == Approx(9.0 * 0.78).epsilon(tolerance));

    isotopes.clear();
    material = "Xe2Ba{Ba[130](0.44),Ba[132](0.56)}";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("Xe[124]") == Approx(2.0 * 0.0010).epsilon(tolerance));
    CHECK(isotopes.at("Xe[126]") == Approx(2.0 * 0.0009).epsilon(tolerance));
    CHECK(isotopes.at("Xe[128]") == Approx(2.0 * 0.0191).epsilon(tolerance));
    CHECK(isotopes.at("Xe[129]") == Approx(2.0 * 0.2640).epsilon(tolerance));
    CHECK(isotopes.at("Xe[130]") == Approx(2.0 * 0.0410).epsilon(tolerance));
    CHECK(isotopes.at("Xe[131]") == Approx(2.0 * 0.2120).epsilon(tolerance));
    CHECK(isotopes.at("Xe[132]") == Approx(2.0 * 0.2690).epsilon(tolerance));
    CHECK(isotopes.at("Xe[134]") == Approx(2.0 * 0.1040).epsilon(tolerance));
    CHECK(isotopes.at("Xe[136]") == Approx(2.0 * 0.0890).epsilon(tolerance));
    CHECK(isotopes.at("Ba[130]") == Approx(0.44).epsilon(tolerance));
    CHECK(isotopes.at("Ba[132]") == Approx(0.56).epsilon(tolerance));

    isotopes.clear();
    material = "Ar{Ar[36](0.2),Ar[38](0.4),Ar[40](0.4)}";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("Ar[36]") == Approx(0.2).epsilon(tolerance));
    CHECK(isotopes.at("Ar[38]") == Approx(0.4).epsilon(tolerance));
    CHECK(isotopes.at("Ar[40]") == Approx(0.4).epsilon(tolerance));

    // Check that a mistake in the ratio throws
    isotopes.clear();
    material = "Ar{Ar[36](0.4),Ar[38](0.4),Ar[40](0.4)}";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check that an unknown isotope throws
    isotopes.clear();
    material = "Ar{Ar[36](0.2),Ar[444](0.4),Ar[40](0.4)}";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check that typos throws

    // ';' instead of ',' in the isotope separator
    isotopes.clear();
    material = "Ar{Ar[36](0.2);Ar[38](0.4),Ar[40](0.4)}";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // '' instead of ',' in the isotope separator
    isotopes.clear();
    material = "Ar{Ar[36](0.2)Ar[38](0.4)Ar[40](0.4)}";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // non-sense character
    isotopes.clear();
    material = "Ar{Ar[36](0.2),vxcvAr[38](0.4),nAr[40](0.4)}";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    isotopes.clear();
    material = "Ar{Ar[36](0.2);Ar[38](0.4),Ar[40](0.4)}";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check chemical compounds

    isotopes.clear();
    material = "CH4";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("C[12]") == Approx(0.98900).epsilon(tolerance));
    CHECK(isotopes.at("C[13]") == Approx(0.01100).epsilon(tolerance));
    CHECK(isotopes.at("H[1]") == Approx(4.0 * 0.99985).epsilon(tolerance));
    CHECK(isotopes.at("H[2]") == Approx(4.0 * 0.00015).epsilon(tolerance));

    isotopes.clear();
    material = "H2O";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("O[16]") == Approx(0.99762).epsilon(tolerance));
    CHECK(isotopes.at("O[17]") == Approx(0.00038).epsilon(tolerance));
    CHECK(isotopes.at("O[18]") == Approx(0.00200).epsilon(tolerance));
    CHECK(isotopes.at("H[1]") == Approx(2.0 * 0.99985).epsilon(tolerance));
    CHECK(isotopes.at("H[2]") == Approx(2.0 * 0.00015).epsilon(tolerance));

    isotopes.clear();
    material = "C2H{H[1](0.8),H[2](0.2)}3N";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("C[12]") == Approx(2.0 * 0.98900).epsilon(tolerance));
    CHECK(isotopes.at("C[13]") == Approx(2.0 * 0.01100).epsilon(tolerance));
    CHECK(isotopes.at("H[1]") == Approx(0.8 * 3.0).epsilon(tolerance));
    CHECK(isotopes.at("H[2]") == Approx(0.2 * 3.0).epsilon(tolerance));
    CHECK(isotopes.at("N[14]") == Approx(0.9963).epsilon(tolerance));
    CHECK(isotopes.at("N[15]") == Approx(0.0037).epsilon(tolerance));

    // Typo when defining the stoichiometry
    isotopes.clear();
    material = "CH%4";
    CHECK_THROWS_AS(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check material = mixture of chemical compounds
    isotopes.clear();
    material = "CH4(0.4);H2O(0.6)";
    CHECK_NOTHROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    CHECK(isotopes.at("C[12]") == Approx(0.4 * 0.98900).epsilon(tolerance));
    CHECK(isotopes.at("C[13]") == Approx(0.4 * 0.01100).epsilon(tolerance));
    CHECK(isotopes.at("H[1]") == Approx(2.8 * 0.99985).epsilon(tolerance));
    CHECK(isotopes.at("H[2]") == Approx(2.8 * 0.00015).epsilon(tolerance));
    CHECK(isotopes.at("O[16]") == Approx(0.6 * 0.99762).epsilon(tolerance));
    CHECK(isotopes.at("O[17]") == Approx(0.6 * 0.00038).epsilon(tolerance));
    CHECK(isotopes.at("O[18]") == Approx(0.6 * 0.00200).epsilon(tolerance));
}
