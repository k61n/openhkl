#include <iostream>
#include <algorithm>
#include <iomanip>

#include "ElementManager.h"
#include "Error.h"
#include "MaterialFromStoichiometry.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

IMaterial* MaterialFromStoichiometry::create(const std::string& name, ChemicalState state)
{
	return (new MaterialFromStoichiometry(name,state));
}

MaterialFromStoichiometry::MaterialFromStoichiometry(const std::string& name) : IMaterial(name)
{
}

MaterialFromStoichiometry::MaterialFromStoichiometry(const std::string& name, ChemicalState state) : IMaterial(name,state)
{
}

MaterialFromStoichiometry::MaterialFromStoichiometry(const std::string& name, const std::string& strState) : IMaterial(name,strState)
{
}

MaterialFromStoichiometry::~MaterialFromStoichiometry()
{
}

BuildingMode MaterialFromStoichiometry::getBuildingMode() const
{
	return BuildingMode::Stoichiometry;
}

double MaterialFromStoichiometry::getMassDensity() const
{
	return _massDensity;
}

void MaterialFromStoichiometry::setMassDensity(double massDensity)
{
	if (massDensity<=0)
		throw SX::Kernel::Error<MaterialFromStoichiometry>("Invalid density value.");

	_massDensity=massDensity;
}

void MaterialFromStoichiometry::addElement(sptrElement element, double stoichiometry)
{
	if (stoichiometry<0)
		throw SX::Kernel::Error<MaterialFromStoichiometry>("Invalid value for stoichiometry");

	std::string eName=element->getName();

	auto it=_contents.find(eName);
	if (it!=_contents.end())
		it->second += stoichiometry;
	else
	{
		_elements.insert(strToElementPair(eName,element));
		_contents.insert(strToDoublePair(eName,stoichiometry));
	}

	return;
}

void MaterialFromStoichiometry::addElement(const std::string& name, double stoichiometry)
{
	ElementManager* mgr=ElementManager::Instance();

	sptrElement el=mgr->getElement(name);

	addElement(el,stoichiometry);

	return;
}

void MaterialFromStoichiometry::addMaterial(sptrMaterial material, double stoichiometry)
{

	for (const auto& p : material->getMolarFractions())
		addElement(p.first,stoichiometry*p.second);

	//! If the material to add has a different chemical state then set the state as a mixture of chemical states
	if (_state != material->getState())
		_state=ChemicalState::Unknown;
}

strToDoubleMap MaterialFromStoichiometry::getMassFractions() const
{
	strToDoubleMap massFractions;

	double totalMass=0.0;
	for (auto p : _contents)
	{
		double prod=p.second*_elements.at(p.first)->getMolarMass();
		totalMass+=prod;
		massFractions.insert(strToDoublePair(p.first,prod));
	}
	for (auto& p : massFractions)
		p.second/=totalMass;

	return massFractions;
}

strToDoubleMap MaterialFromStoichiometry::getMolarFractions() const
{
	strToDoubleMap molarFractions;

	double nAtoms=std::accumulate(std::begin(_contents),
			                       std::end(_contents),
			                       0.0,
			                       [](double previous, const strToDoublePair& p) {return previous+p.second;});
	for (auto it=_contents.begin();it!=_contents.end();++it)
		molarFractions.insert(strToDoublePair(it->first,it->second/nAtoms));

	return molarFractions;
}

strToDoubleMap MaterialFromStoichiometry::getPartialPressures() const
{
	if (_state!=ChemicalState::Gaz)
		throw SX::Kernel::Error<MaterialFromStoichiometry>("Invalid material state.");

	if (_temperature < 1.0-9)
		throw SX::Kernel::Error<MaterialFromStoichiometry>("Invalid temperature.");

	if (_massDensity < 1.0-9)
		throw SX::Kernel::Error<MaterialFromStoichiometry>("Invalid density.");


	strToDoubleMap partialPressures=getMolarFractions();
	double fact(0.0);
	for (const auto& p : partialPressures)
		fact+=p.second*_elements.at(p.first)->getMolarMass();
	double totalPressure(_massDensity*SX::Units::R/_temperature/fact);

	for (auto& p : partialPressures)
		p.second*=totalPressure;

	return partialPressures;
}

strToDoubleMap MaterialFromStoichiometry::getStoichiometry() const
{
	return _contents;
}

double MaterialFromStoichiometry::getMolarMass() const
{
	double mm(0.0);

	auto cit=_contents.cbegin();
	for (const auto& e : _elements)
		mm+=((cit++)->second)*(e.second->getMolarMass());

	return mm;
}

std::string MaterialFromStoichiometry::getChemicalFormula() const
{
	std::ostringstream cf;

	auto it=_contents.cbegin();
	for (const auto& p : _elements)
		cf<<p.first<<std::setiosflags(std::ios::fixed)<<std::setprecision(2)<<(it++)->second;

	return cf.str();
}

void MaterialFromStoichiometry::writeToXML(property_tree::ptree& parent) const
{
	property_tree::ptree& node=parent.add("material","");
	node.put("<xmlattr>.name",_name);
	node.put("<xmlattr>.chemical_state",stateToStr.at(_state));
	node.put("<xmlattr>.building_mode","stoichiometry");
	node.put<double>("mass_density",getMassDensity());

	for (const auto& el : getMassFractions())
	{
		property_tree::ptree& elnode=node.add("element","");
		elnode.put("<xmlattr>.name",el.first);
		elnode.put<double>("contents",el.second);
	}
}

void MaterialFromStoichiometry::print(std::ostream& os) const
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
		os<<"Stoichiometry:"<<std::endl;
		auto ait=_contents.begin();
		for (const auto& p : _elements)
		{
			os<<"\t-"<<std::setw(maxSize)<<std::setiosflags(std::ios::left)<<p.second->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<ait->second<<std::endl;
			os<< *(p.second) << std::endl;
			std::cout<<std::resetiosflags(std::ios::right);
			++ait;
		}
	}
}

} // end namespace Chemistry

} // end namespace SX
