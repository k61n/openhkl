#define BOOST_TEST_MODULE "Test Element"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <nsxlib/chemistry/Element.h>
#include <nsxlib/kernel/Error.h>
#include <nsxlib/utils/Units.h>

using namespace SX::Chemistry;
using namespace SX::Units;

const double tolerance=1.0e-6;

BOOST_AUTO_TEST_CASE(Test_Element)
{
    UnitsManager* um=UnitsManager::Instance();

    // Builds the natural hydrogen Element directly from the isotopes registry/database
    Element hydrogen("H");
    // Checks that the number of neutrons is OK
    BOOST_CHECK_CLOSE(hydrogen.getNNeutrons(),(1*0.00015),tolerance);
    // Checks that the molar mass is OK
    BOOST_CHECK_CLOSE(hydrogen.getMolarMass(),(1.007825032*0.99985+2.014101778*0.00015)*um->get("g_per_mole"),tolerance);

    // Builds an empty element and fills it will isotopes
    Element uranium("natU");
    uranium.addIsotope("U[235]");
    uranium.addIsotope("U[238]");
    // Checks that adding an incompatible isotope throws
    BOOST_CHECK_THROW(uranium.addIsotope("C[12]"),std::runtime_error);
}
