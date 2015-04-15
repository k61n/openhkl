#include <iostream>
#include <algorithm>
#include <iomanip>

#include "ElementManager.h"
#include "Error.h"
#include "MaterialFromMolarFractions.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

IMaterial* MaterialFromMolarFractions::create(const std::string& name, ChemicalState state)
{
	return (new MaterialFromMolarFractions(name,state));
}

MaterialFromMolarFractions::MaterialFromMolarFractions(const std::string& name) : IMaterial(name)
{
}

MaterialFromMolarFractions::MaterialFromMolarFractions(const std::string& name, ChemicalState state) : IMaterial(name,state)
{
}

MaterialFromMolarFractions::MaterialFromMolarFractions(const std::string& name, const std::string& strState) : IMaterial(name,strState)
{
}

MaterialFromMolarFractions::~MaterialFromMolarFractions()
{
}

BuildingMode MaterialFromMolarFractions::getBuildingMode() const
{
	return BuildingMode::MolarFractions;
}

double MaterialFromMolarFractions::getMassDensity() const
{
	return _massDensity;
}

void MaterialFromMolarFractions::setMassDensity(double massDensity)
{
	if (massDensity<=0)
		throw SX::Kernel::Error<MaterialFromMolarFractions>("Invalid density value.");

	_massDensity=massDensity;
}

double MaterialFromMolarFractions::getMolarMass() const
{
	double mm(0.0);

	auto cit=_contents.cbegin();
	for (const auto& e : _elements)
		mm+=((cit++)->second)*(e.second->getMolarMass());

	return mm;
}

std::string MaterialFromMolarFractions::getChemicalFormula() const
{
	std::ostringstream cf;

	auto it=_contents.cbegin();
	for (const auto& p : _elements)
		cf<<p.first<<std::setiosflags(std::ios::fixed)<<std::setprecision(2)<<(it++)->second;

	return cf.str();
}

void MaterialFromMolarFractions::addElement(sptrElement element, double molarFraction)
{
	if (molarFraction<=0 || molarFraction>1)
		throw SX::Kernel::Error<MaterialFromMolarFractions>("Invalid value for molar fraction.");
	double sum=std::accumulate(std::begin(_contents),
								std::end(_contents),
								molarFraction,
								[](double previous, const strToDoublePair& p) { return previous+p.second;});
	if (sum>(1.00000001))
		throw SX::Kernel::Error<MaterialFromMolarFractions>("The sum of mole fractions exceeds 1.0");

	std::string eName=element->getName();

	auto it=_contents.find(eName);
	if (it!=_contents.end())
		it->second += molarFraction;
	else
	{
		_elements.insert(strToElementPair(eName,element));
		_contents.insert(strToDoublePair(eName,molarFraction));
	}

	return;
}

void MaterialFromMolarFractions::addElement(const std::string& name, double molarFraction)
{
	ElementManager* mgr=ElementManager::Instance();

	sptrElement el=mgr->getElement(name);

	addElement(el,molarFraction);

	return;
}

void MaterialFromMolarFractions::addMaterial(sptrMaterial material, double molarFraction)
{

	for (const auto& p : material->getMolarFractions())
		addElement(p.first,molarFraction*p.second);

	//! If the material to add has a different chemical state then set the state as a mixture of chemical states
	if (_state != material->getState())
		_state=ChemicalState::Unknown;
}

strToDoubleMap MaterialFromMolarFractions::getMassFractions() const
{
	strToDoubleMap massFractions;

	double fact=0.0;
	for (auto p : _contents)
	{
		double prod=p.second*_elements.at(p.first)->getMolarMass();
		fact+=prod;
		massFractions.insert(strToDoublePair(p.first,prod));
	}
	for (auto& p : massFractions)
		p.second/=fact;

	return massFractions;
}

strToDoubleMap MaterialFromMolarFractions::getMolarFractions() const
{
	return _contents;
}

strToDoubleMap MaterialFromMolarFractions::getPartialPressures() const
{
	if (_state!=ChemicalState::Gaz)
		throw SX::Kernel::Error<MaterialFromMolarFractions>("Invalid material state.");

	if (_temperature < 1.0-9)
		throw SX::Kernel::Error<MaterialFromMolarFractions>("Invalid temperature.");

	if (_massDensity < 1.0-9)
		throw SX::Kernel::Error<MaterialFromMolarFractions>("Invalid density.");

	strToDoubleMap partialPressures(_contents);
	double fact(0.0);
	for (const auto& p : partialPressures)
		fact+=p.second*_elements.at(p.first)->getMolarMass();
	double totalPressure(_massDensity*SX::Units::R/_temperature/fact);

	for (auto& p : partialPressures)
		p.second*=totalPressure;

	return partialPressures;
}

strToDoubleMap MaterialFromMolarFractions::getStoichiometry() const
{
	strToDoubleMap stochiometry(_contents);

	auto it=std::min_element(stochiometry.begin(),
			                          stochiometry.end(),
			                          [] (const strToDoublePair & p1, const strToDoublePair & p2) -> bool {return (p1.second < p2.second);});

	double minFraction=it->second;

	for (auto& p : stochiometry)
		p.second/=minFraction;

	return stochiometry;
}

void MaterialFromMolarFractions::writeToXML(property_tree::ptree& parent) const
{
	property_tree::ptree& node=parent.add("material","");
	node.put("<xmlattr>.name",_name);
	node.put("<xmlattr>.chemical_state",stateToStr.at(_state));
	node.put("<xmlattr>.building_mode","molar fractions");
	node.put<double>("mass_density",getMassDensity());

	for (const auto& el : getMassFractions())
	{
		property_tree::ptree& elnode=node.add("element","");
		elnode.put("<xmlattr>.name",el.first);
		elnode.put<double>("contents",el.second);
	}
}

void MaterialFromMolarFractions::print(std::ostream& os) const
{
	os<<"Material "<<_name<<" --> State="<<stateToStr.at(_state)<<std::endl;
	if (_elements.empty())
		os<<"Currently empty"<<std::endl;
	else
	{
		unsigned int maxSize=0;
		for (const auto& p : _elements)
		{
			unsigned int nameSize=p.second->getName().size();
			if (nameSize > maxSize)
				maxSize=nameSize;
		}
		os<<"Molar fractions:"<<std::endl;
		for (const auto& p : _elements)
		{
			os<<"\t-"<<std::setw(maxSize)<<std::setiosflags(std::ios::left)<<p.second->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<p.second<<std::endl;
			os<<std::resetiosflags(std::ios::right);
		}
	}
}

} // end namespace Chemistry

} // end namespace SX
