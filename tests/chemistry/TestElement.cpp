#define BOOST_TEST_MODULE "Test Element"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "Error.h"
#include "Element.h"

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_Element)
{
	// Create a new element based on its natural isotopes
	Element el1("H","H");
	// Check that the number of natural isotopes of this element is OK (H[1],H[2],H[3])
	BOOST_CHECK_EQUAL(el1.getNIsotopes(),3);

	// Register this element and check that elements registry has been modified
	BOOST_CHECK_EQUAL(Element::getNRegisteredElements(),0);
	Element::registerElement(&el1);
	BOOST_CHECK_EQUAL(Element::getNRegisteredElements(),1);

	// Check that registering an element with an name that is already in use throws
	Element el2("H","He");
	BOOST_CHECK_THROW(Element::registerElement(&el2),SX::Kernel::Error<Element>)

	// Check for chemical consistency (same Z) when adding an isotopes to an element
	Element el3("U");
	el3.addIsotope("U[235]");
	BOOST_CHECK_THROW(el3.addIsotope("C[12]"),SX::Kernel::Error<Element>)

	// Check that building an element from the database which is unknown throws
	BOOST_CHECK_THROW(Element::buildFromDatabase("XXX"),SX::Kernel::Error<Element>);

	// Build an element from the elements database and check that the registry has been modified
	Element* uranium=Element::buildFromDatabase("U");
	BOOST_CHECK_EQUAL(Element::getNRegisteredElements(),2);
}
