#define BOOST_TEST_MODULE "Test Material"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>

#include "Units.h"
#include "Element.h"
#include "Material.h"
#include "Units.h"

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_Material)
{

	Material* methane=Material::buildFromDatabase("CH4");
	std::cout<<Element::getNRegisteredElements()<<std::endl;

//
//	for (auto it : methane->getMoleFractions())
//		std::cout<<it.first->getName()<<" "<<it.second<<std::endl;
//
//	Material* mat1=Material::buildFromDatabase("CH41");
//
//	for (auto it : mat1->getMassFractions())
//		std::cout<<it.first->getName()<<" "<<it.second<<std::endl;
//
	Material* water=Material::buildFromDatabase("H2O");
	std::cout<<Element::getNRegisteredElements()<<std::endl;
//
////	for (auto it : water->getMoleFractions())
////		std::cout<<it.first->getName()<<" "<<it.second<<std::endl;
////
//	std::cout<<"WATER"<<std::endl;
//	for (auto it : water->getNAtomsPerVolume())
//		std::cout<<it.first->getName()<<" "<<it.second<<std::endl;

	Material* mixture=new Material("mixture",1.235,Material::FillingMode::MassFraction);
	mixture->addMaterial(methane,0.5);
	mixture->addMaterial(water,0.5);
	std::cout<<Element::getNRegisteredElements()<<std::endl;
	std::cout<<"mixture"<<std::endl;
	for (auto it : mixture->getMassFractions())
		std::cout<<it.first->getName()<<" "<<it.second<<std::endl;

	Material* mixture1=Material::buildFromDatabase("mixture");
	std::cout<<"mixture1"<<std::endl;
	for (auto it : mixture1->getMassFractions())
		std::cout<<it.first->getName()<<" "<<it.second<<std::endl;

	std::cout<<Element::getNRegisteredElements()<<std::endl;



	//
//	std::cout<<"density"<<std::endl;
//	for (auto it : mixture.getNAtomsPerVolume())
//		std::cout<<it.first->getName()<<" "<<it.second<<std::endl;

//	BOOST_CHECK_EQUAL(mat->getNElements(),2);
//
//	const elementVector& elements=mat->getElements();
//	BOOST_CHECK_EQUAL(elements[0]->getNIsotopes(),2);
//	BOOST_CHECK_EQUAL(elements[1]->getNIsotopes(),3);

}
