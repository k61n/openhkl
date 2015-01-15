#define BOOST_TEST_MODULE "Test Material"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <map>

#include "Units.h"
#include "Isotope.h"
#include "Element.h"
#include "Material.h"
#include "Units.h"

using namespace SX::Chemistry;

const double tolerance=1;

BOOST_AUTO_TEST_CASE(Test_Material)
{

	// Build a methane material dynamically
	Material methane("methane",Material::State::Gaz,Material::FillingMode::NumberOfAtoms);
	Element carbon("carbon","C");
	Element hydrogen("hydrogen","H");
	methane.addElement(&carbon,1);
	methane.addElement(&hydrogen,4);
	methane.setDensity(1.235);

	// Check that the registry of elements has been correctly updated
	BOOST_CHECK_EQUAL(Element::getNRegisteredElements(),2);

	// Check that the registry of materials has been correctly updated
	BOOST_CHECK_EQUAL(Material::getNRegisteredMaterials(),1);

	std::map<Element*,double> massFractions=methane.getMassFractions();
	BOOST_CHECK_CLOSE(massFractions.at(&carbon),0.75,tolerance);
	BOOST_CHECK_CLOSE(massFractions.at(&hydrogen),0.25,tolerance);

	// Build another methane material dynamically with a different filling method
	Material methane1("methane1",Material::State::Gaz,Material::FillingMode::MoleFraction);
	methane1.addElement(&carbon,0.2);
	methane1.addElement(&hydrogen,0.8);
	methane1.setDensity(1.235);
	// Check that this corresponds (chemically) to the same material
	BOOST_CHECK_EQUAL(methane==methane1,true);

	// Build a water material dynamically
	Material water("water",Material::State::Liquid,Material::FillingMode::MoleFraction);
	Element oxygen("oxygen","O");
	water.addElement(&oxygen,1.0/3.0);
	water.addElement(&hydrogen,2.0/3.0);
	water.setDensity(1.000);

	// Check that the registry of elements has been correctly updated
	BOOST_CHECK_EQUAL(Element::getNRegisteredElements(),3);

	// Check that the registry of materials has been correctly updated
	BOOST_CHECK_EQUAL(Material::getNRegisteredMaterials(),3);

	massFractions=water.getMassFractions();
	BOOST_CHECK_CLOSE(massFractions.at(&oxygen),8.0/9.0,tolerance);
	BOOST_CHECK_CLOSE(massFractions.at(&hydrogen),1.0/9.0,tolerance);

	// Build a mixture of materials dynamically
	Material mixture("mixture",Material::State::Solid,Material::FillingMode::MassFraction);
	mixture.addMaterial(&methane,0.5);
	mixture.addMaterial(&water,0.5);
	mixture.setDensity(3.0);

	// Check that the registry of materials has been correctly updated
	BOOST_CHECK_EQUAL(Material::getNRegisteredMaterials(),4);

	// Build a methane from the materials database
	Material* methane2=Material::buildFromDatabase("CH4");
	// Check that the registry of materials has been correctly updated
	BOOST_CHECK_EQUAL(Material::getNRegisteredMaterials(),5);
	// Check that the registry of elements has not been updated
	BOOST_CHECK_EQUAL(Element::getNRegisteredElements(),3);
	// Check that it corresponds (chemically) to the methane previously defined
	BOOST_CHECK_EQUAL(*methane2==methane,true);

	Material cf4("CF4",Material::State::Gaz,Material::FillingMode::NumberOfAtoms);
	Element fluorine("fluor","F");
	cf4.addElement(&carbon,1);
	cf4.addElement(&fluorine,4);

	Material he3("He3",Material::State::Gaz,Material::FillingMode::NumberOfAtoms);
	Element helium3("helium3");
	helium3.addIsotope(Isotope::buildFromDatabase("He[3]"),1.0);
	he3.addElement(&helium3,1);

	Material* gazmix= new Material("HeCF4",Material::State::Gaz,Material::FillingMode::PartialPressure);
	gazmix->addMaterial(&cf4,2*SX::Units::Bar);
	gazmix->addMaterial(&he3,10*SX::Units::Bar);
	gazmix->setTemperature(290.0);

}
