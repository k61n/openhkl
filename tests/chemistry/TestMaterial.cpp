#define BOOST_TEST_MODULE "Test Material"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "Element.h"
#include "ElementManager.h"
#include "Error.h"
#include "IMaterial.h"
#include "MaterialManager.h"

const double tolerance=1.0e-9;

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_Material)
{
	MaterialManager* mmgr=MaterialManager::Instance();
	ElementManager* emgr=ElementManager::Instance();

	// Checks that setting the materials database to a wrong path throws
	BOOST_CHECK_THROW(mmgr->setDatabasePath("/fsdfs/fsdfsd/blablabla.xml"),SX::Kernel::Error<MaterialManager>);

	// Checks that setting the materials database to a correct path does not throw
	BOOST_CHECK_NO_THROW(mmgr->setDatabasePath("./materials.xml"));

	// Builds the methane molecule from the materials XML database
	SX::Chemistry::sptrMaterial dbMethane=mmgr->getMaterial("db_methane");

	// Check that the registry of materials has been correctly updated
	BOOST_CHECK_EQUAL(mmgr->getNMaterials(),1);

	// Checks that the methane molecule is made of two elements each of them being a composite of isotopes
	BOOST_CHECK_EQUAL(dbMethane->getNElements(),2);
	BOOST_CHECK_EQUAL((*dbMethane)["carbon"]->getNIsotopes(),2);
	BOOST_CHECK_EQUAL((*dbMethane)["hydrogen"]->getNIsotopes(),3);
	BOOST_CHECK_THROW((*dbMethane)["blabla"]->getNIsotopes(),SX::Kernel::Error<IMaterial>);

	// Checks the mole fractions of this methane molecule
	contentsMap moleFractions=dbMethane->getMolarFractions();
	BOOST_CHECK_EQUAL(moleFractions["carbon"],0.2);
	BOOST_CHECK_EQUAL(moleFractions["hydrogen"],0.8);

	// Checks the number of atoms of this methane molecule
	contentsMap numberOfAtoms=dbMethane->getStoichiometry();
	BOOST_CHECK_EQUAL(numberOfAtoms["carbon"],1.00);
	BOOST_CHECK_EQUAL(numberOfAtoms["hydrogen"],4.00);

	// Checks the mass fractions of this methane molecule
	double mCarbon = (*dbMethane)["carbon"]->getMolarMass();
	double mHydrogen = (*dbMethane)["hydrogen"]->getMolarMass();
	double mTotal = mCarbon + 4.0*mHydrogen;
	contentsMap massFractions=dbMethane->getMassFractions();
	BOOST_CHECK_CLOSE(massFractions["carbon"],mCarbon/mTotal,tolerance);
	BOOST_CHECK_CLOSE(massFractions["hydrogen"],4.0*mHydrogen/mTotal,tolerance);

	// Build a methane material dynamically and checks that its contents is the same than the one built from the database
	SX::Chemistry::sptrMaterial methane= mmgr->buildEmptyMaterial("methane",IMaterial::State::Gaz,IMaterial::BuildingMode::Stoichiometry);
	methane->addElement(emgr->getElement("carbon"),1);
	methane->addElement(emgr->getElement("hydrogen"),4);
	methane->setMassDensity(1.235);
	contentsMap massFractions1=methane->getMassFractions();
	BOOST_CHECK_EQUAL(massFractions["carbon"],massFractions["carbon"]);
	BOOST_CHECK_EQUAL(massFractions["hydrogen"],massFractions["hydrogen"]);

	// Check that the registry of materials has been correctly updated
	BOOST_CHECK_EQUAL(mmgr->getNMaterials(),2);

	// Build a mixture of material from the XML database
	SX::Chemistry::sptrMaterial dbMixture=mmgr->getMaterial("db_mixture");

	SX::Chemistry::sptrMaterial b4c=mmgr->buildMaterialFromChemicalFormula("B4C",IMaterial::State::Solid);
	BOOST_CHECK_EQUAL((*b4c)["C"]->getNIsotopes(),2);
	BOOST_CHECK_EQUAL((*b4c)["B"]->getNIsotopes(),2);
	moleFractions=b4c->getMolarFractions();
	BOOST_CHECK_EQUAL(moleFractions["C"],0.2);
	BOOST_CHECK_EQUAL(moleFractions["B"],0.8);

	mmgr->saveRegistry("materials_new.xml");
}
