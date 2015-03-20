#include <algorithm>
#include <iomanip>

#include "ElementManager.h"
#include "Error.h"
#include "MaterialFromPartialPressures.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

IMaterial* MaterialFromPartialPressures::create(const std::string& name, ChemicalState state)
{
	return (new MaterialFromPartialPressures(name,state));
}

MaterialFromPartialPressures::MaterialFromPartialPressures(const std::string& name) : IMaterial(name, ChemicalState::Gaz)
{
}

MaterialFromPartialPressures::MaterialFromPartialPressures(const std::string& name, ChemicalState state) : IMaterial(name)
{
	if (state!=ChemicalState::Gaz)
		throw SX::Kernel::Error<MaterialFromPartialPressures>("Invalid chemical state.");
	_state=state;
}

MaterialFromPartialPressures::MaterialFromPartialPressures(const std::string& name, const std::string& strState) : IMaterial(name)
{
	ChemicalState state=strToState.at(strState);
	if (state!=ChemicalState::Gaz)
		throw SX::Kernel::Error<MaterialFromPartialPressures>("Invalid chemical state.");
	_state=state;
}

MaterialFromPartialPressures::~MaterialFromPartialPressures()
{
}

BuildingMode MaterialFromPartialPressures::getBuildingMode() const
{
	return BuildingMode::PartialPressures;
}

double MaterialFromPartialPressures::getMassDensity() const
{
	if (_temperature<=0)
		throw SX::Kernel::Error<MaterialFromPartialPressures>("Invalid temperature value");

	double totalPressure=std::accumulate(std::begin(_contents),
			                              std::end(_contents),
			                              0.0,
			                              [](double previous, const strToDoublePair& p) {return previous+p.second;});

	// The molar density (eta) is computed as eta=P/RT
	double molarDensity=totalPressure/SX::Units::R/_temperature;

	contentsMap molarFractions=getMolarFractions();

	// The mass density (mu) is computed as mu=eta*sum_i(eta_i*M_i) where eta_i and M_i are respectively the molar fraction and molar mass of element i
	double massDensity=0.0;
	for (const auto& p : molarFractions)
		massDensity+=p.second*static_cast<double>(_elements.at(p.first)->getMolarMass());
	massDensity*=molarDensity;

	return massDensity;
}

void MaterialFromPartialPressures::setMassDensity(double massDensity)
{
}

double MaterialFromPartialPressures::getMolarMass() const
{
	double mm(0.0);

	auto molarFractions=getMolarFractions();
	auto cit=molarFractions.cbegin();
	for (const auto& e : _elements)
		mm+=((cit++)->second)*(e.second->getMolarMass());

	return mm;
}

std::string MaterialFromPartialPressures::getChemicalFormula() const
{
	std::ostringstream cf;

	auto molarFractions=getMolarFractions();
	auto it=molarFractions.cbegin();
	for (const auto& p : _elements)
		cf<<p.first<<std::setiosflags(std::ios::fixed)<<std::setprecision(2)<<(it++)->second;

	return cf.str();
}

void MaterialFromPartialPressures::addElement(sptrElement element, double partialPressure)
{
	if (partialPressure<=0)
		throw SX::Kernel::Error<MaterialFromPartialPressures>("Invalid value for partial pressure");

	std::string eName=element->getName();

	auto it=_contents.find(eName);
	if (it!=_contents.end())
		it->second += partialPressure;
	else
	{
		_elements.insert(strToElementPair(eName,element));
		_contents.insert(strToDoublePair(eName,partialPressure));
	}

	return;
}

void MaterialFromPartialPressures::addElement(const std::string& name, double partialPressure)
{
	ElementManager* mgr=ElementManager::Instance();

	sptrElement element=mgr->getElement(name);

	addElement(element,partialPressure);

	return;
}

void MaterialFromPartialPressures::addMaterial(sptrMaterial material, double partialPressure)
{

	ChemicalState otherState=material->getState();
	if (otherState!=ChemicalState::Gaz)
		throw SX::Kernel::Error<MaterialFromPartialPressures>("Invalid chemical state.");

	for (const auto& p : material->getMolarFractions())
		addElement(p.first,partialPressure*p.second);

	//! If the material to add has a different chemical state then set the state as a mixture of chemical states
	if (_state != material->getState())
		_state=ChemicalState::Unknown;
}

strToDoubleMap MaterialFromPartialPressures::getMassFractions() const
{
	strToDoubleMap massFractions;

	double fact=0.0;
	for (auto p : getMolarFractions())
	{
		double prod=p.second*_elements.at(p.first)->getMolarMass();
		fact+=prod;
		massFractions.insert(strToDoublePair(p.first,prod));
	}
	for (auto& p : massFractions)
		p.second/=fact;

	return massFractions;
}

strToDoubleMap MaterialFromPartialPressures::getMolarFractions() const
{
	double totalPressure=std::accumulate(std::begin(_contents),
			                              std::end(_contents),
			                              0.0,
			                              [](double previous, const strToDoublePair& p) { return previous+p.second;});

	strToDoubleMap molarFractions(_contents);
	for (auto& p : molarFractions)
		p.second/=totalPressure;

	return molarFractions;
}

strToDoubleMap MaterialFromPartialPressures::getPartialPressures() const
{
	return _contents;
}

strToDoubleMap MaterialFromPartialPressures::getStoichiometry() const
{
	strToDoubleMap stochiometry(getMolarFractions());

	auto it=std::min_element(stochiometry.begin(),
			                          stochiometry.end(),
			                          [] (const strToDoublePair & p1, const strToDoublePair & p2) -> bool {return (p1.second < p2.second);});

	double minFraction=it->second;

	for (auto& f : stochiometry)
		f.second/=minFraction;

	return stochiometry;
}

void MaterialFromPartialPressures::writeToXML(property_tree::ptree& parent) const
{
	property_tree::ptree& node=parent.add("material","");
	node.put("<xmlattr>.name",_name);
	node.put("<xmlattr>.chemical_state",stateToStr.at(_state));
	node.put("<xmlattr>.building_mode","partial pressures");

	for (const auto& el : getMassFractions())
	{
		property_tree::ptree& elnode=node.add("element","");
		elnode.put("<xmlattr>.name",el.first);
		elnode.put<double>("contents",el.second);
	}
}

void MaterialFromPartialPressures::print(std::ostream& os) const
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
		os<<"Partial pressures:"<<std::endl;
		for (const auto& p : _elements)
		{
			os<<"\t-"<<std::setw(maxSize)<<std::setiosflags(std::ios::left)<<p.second->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<p.second<<std::endl;
			std::cout<<std::resetiosflags(std::ios::right);
		}
	}
}

} // end namespace Chemistry

} // end namespace SX
