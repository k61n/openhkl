#define BOOST_TEST_MODULE "Test Material"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/chemistry/Element.h>
#include <nsxlib/kernel/Error.h>
#include <nsxlib/chemistry/Material.h>
#include <nsxlib/chemistry/MaterialManager.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1.0e-9;

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_Material)
{
    MaterialManager* mmgr=MaterialManager::Instance();

    // Checks that setting the materials database to a wrong path throws
    BOOST_CHECK_THROW(mmgr->setDatabasePath("/fsdfs/fsdfsd/blablabla.xml"),SX::Kernel::Error<MaterialManager>);

    sptrMaterial methane= mmgr->buildEmptyMaterial("methane",BuildingMode::Stoichiometry);
    methane->addElement(std::make_shared<Element>(Element("C")),1);
    methane->addElement(std::make_shared<Element>(Element("H")),4);
    BOOST_CHECK_CLOSE(methane->getMolarMass(),0.0160425,1.0e-2);
    methane->setMassDensity(1.235);

    sptrMaterial water= mmgr->buildEmptyMaterial("water",BuildingMode::Stoichiometry);
    water->addElement(std::make_shared<Element>(Element("O")),1);
    water->addElement(std::make_shared<Element>(Element("H")),2);
    BOOST_CHECK_CLOSE(water->getMolarMass(),0.01801528,1.0e-2);

    sptrMaterial mixture= mmgr->buildEmptyMaterial("mixture",BuildingMode::Stoichiometry);
    mixture->addMaterial(methane,5);
    mixture->addMaterial(water,20);

//    // Checks the mole fractions of the methane and water molecules
//    elementsMap moleFractions=methane->getMolarFractions();
//    BOOST_CHECK_EQUAL(moleFractions[emgr->getElement("C")],0.2);
//    BOOST_CHECK_EQUAL(moleFractions[emgr->getElement("H")],0.8);
//
//    moleFractions=water->getMolarFractions();
//    BOOST_CHECK_EQUAL(moleFractions[emgr->getElement("O")],1.0/3.0);
//    BOOST_CHECK_EQUAL(moleFractions[emgr->getElement("H")],2.0/3.0);
//
//    // Checks the mass fractions of the methane and water molecules
//    double mCarbon = emgr->getElement("C")->getMolarMass();
//    double mHydrogen = emgr->getElement("H")->getMolarMass();
//    double mTotal = mCarbon + 4.0*mHydrogen;
//    elementsMap massFractions=methane->getMassFractions();
//    BOOST_CHECK_CLOSE(massFractions[emgr->getElement("C")],mCarbon/mTotal,tolerance);
//    BOOST_CHECK_CLOSE(massFractions[emgr->getElement("H")],4.0*mHydrogen/mTotal,tolerance);
//    double mOxygen = emgr->getElement("O")->getMolarMass();
//    mTotal = mOxygen + 2.0*mHydrogen;
//    massFractions=water->getMassFractions();
//    BOOST_CHECK_CLOSE(massFractions[emgr->getElement("O")],mOxygen/mTotal,tolerance);
//    BOOST_CHECK_CLOSE(massFractions[emgr->getElement("H")],2.0*mHydrogen/mTotal,tolerance);
//
//    // Build a new material with floating stoichiometry
//    SX::Chemistry::sptrMaterial bacl2= mmgr->buildEmptyMaterial("bacl2",BuildingMode::Stoichiometry);
//    bacl2->addElement(emgr->getElement("Ba"),0.95);
//    bacl2->addElement(emgr->getElement("Cl"),0.05);
//    // Build the same material but from a chemical formula
//    SX::Chemistry::sptrMaterial bacl2_1=mmgr->buildMaterialFromChemicalFormula("Ba0.95Cl0.05");
//    // Check that both materials are the same
//    BOOST_CHECK(*bacl2==*bacl2_1);
//
//     mmgr->saveRegistry("materials_new.xml");
//
//    // Checks that setting the materials database to a correct path does not throw
//    BOOST_CHECK_NO_THROW(mmgr->setDatabasePath("./materials_new.xml"));
//
//    mmgr->cleanRegistry();
//
//    sptrMaterial methaneDB = mmgr->getMaterial("methane");
//
//    std::cout<<*mixture<<std::endl;

}
