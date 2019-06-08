#define NSX_SPIRIT_USE_PHOENIX_V3

#include <stdexcept>
#include <string>

#include "core/chemistry/ChemicalFormulaParser.h"
#include "core/chemistry/ChemistryTypes.h"

// This had to be included here to avoid lengthy compile error with
// boost::spirit
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

const double tolerance = 1.0e-9;

NSX_INIT_TEST

int main()
{
    using boost::spirit::qi::blank;
    using boost::spirit::qi::phrase_parse;

    nsx::isotopeContents isotopes;

    nsx::ChemicalFormulaParser<std::string::iterator> parser;

    // Check pure isotopes

    std::string material;

    material = "H[1]";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 1.00, tolerance);

    isotopes.clear();
    material = "U[235]";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("U[235]"), 1.00, tolerance);

    isotopes.clear();
    material = "X";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    isotopes.clear();
    material = "U[420]";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check natural elements

    isotopes.clear();
    material = "Li";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("Li[6]"), 0.075, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Li[7]"), 0.925, tolerance);

    isotopes.clear();
    material = "Br";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("Br[79]"), 0.5069, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Br[81]"), 0.4931, tolerance);

    isotopes.clear();
    material = "Z";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check isotope mixtures

    isotopes.clear();
    material = "C2H{H[1](0.22),H[2](0.78)}9";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("C[12]"), 2.0 * 0.98900, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("C[13]"), 2.0 * 0.01100, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 9.0 * 0.22, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[2]"), 9.0 * 0.78, tolerance);

    isotopes.clear();
    material = "Xe2Ba{Ba[130](0.44),Ba[132](0.56)}";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("Xe[124]"), 2.0 * 0.0010, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[126]"), 2.0 * 0.0009, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[128]"), 2.0 * 0.0191, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[129]"), 2.0 * 0.2640, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[130]"), 2.0 * 0.0410, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[131]"), 2.0 * 0.2120, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[132]"), 2.0 * 0.2690, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[134]"), 2.0 * 0.1040, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Xe[136]"), 2.0 * 0.0890, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Ba[130]"), 0.44, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Ba[132]"), 0.56, tolerance);

    isotopes.clear();
    material = "Ar{Ar[36](0.2),Ar[38](0.4),Ar[40](0.4)}";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("Ar[36]"), 0.2, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Ar[38]"), 0.4, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("Ar[40]"), 0.4, tolerance);

    // Check that a mistake in the ratio throws
    isotopes.clear();
    material = "Ar{Ar[36](0.4),Ar[38](0.4),Ar[40](0.4)}";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check that an unknown isotope throws
    isotopes.clear();
    material = "Ar{Ar[36](0.2),Ar[444](0.4),Ar[40](0.4)}";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check that typos throws

    // ';' instead of ',' in the isotope separator
    isotopes.clear();
    material = "Ar{Ar[36](0.2);Ar[38](0.4),Ar[40](0.4)}";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // '' instead of ',' in the isotope separator
    isotopes.clear();
    material = "Ar{Ar[36](0.2)Ar[38](0.4)Ar[40](0.4)}";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // non-sense character
    isotopes.clear();
    material = "Ar{Ar[36](0.2),vxcvAr[38](0.4),nAr[40](0.4)}";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    isotopes.clear();
    material = "Ar{Ar[36](0.2);Ar[38](0.4),Ar[40](0.4)}";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check chemical compounds

    isotopes.clear();
    material = "CH4";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("C[12]"), 0.98900, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("C[13]"), 0.01100, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 4.0 * 0.99985, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[2]"), 4.0 * 0.00015, tolerance);

    isotopes.clear();
    material = "H2O";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("O[16]"), 0.99762, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("O[17]"), 0.00038, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("O[18]"), 0.00200, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 2.0 * 0.99985, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[2]"), 2.0 * 0.00015, tolerance);

    isotopes.clear();
    material = "C2H{H[1](0.8),H[2](0.2)}3N";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("C[12]"), 2.0 * 0.98900, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("C[13]"), 2.0 * 0.01100, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 0.8 * 3.0, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[2]"), 0.2 * 3.0, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("N[14]"), 0.9963, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("N[15]"), 0.0037, tolerance);

    // Typo when defining the stoichiometry
    isotopes.clear();
    material = "CH%4";
    NSX_CHECK_THROW(
        phrase_parse(material.begin(), material.end(), parser, blank, isotopes),
        std::runtime_error);

    // Check material = mixture of chemical compounds
    isotopes.clear();
    material = "CH4(0.4);H2O(0.6)";
    NSX_CHECK_NO_THROW(phrase_parse(material.begin(), material.end(), parser, blank, isotopes));
    NSX_CHECK_CLOSE(isotopes.at("C[12]"), 0.4 * 0.98900, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("C[13]"), 0.4 * 0.01100, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[1]"), 2.8 * 0.99985, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("H[2]"), 2.8 * 0.00015, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("O[16]"), 0.6 * 0.99762, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("O[17]"), 0.6 * 0.00038, tolerance);
    NSX_CHECK_CLOSE(isotopes.at("O[18]"), 0.6 * 0.00200, tolerance);

    return 0;
}
