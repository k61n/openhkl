#include <algorithm>
#include <iomanip>

#include "ElementManager.h"
#include "Error.h"
#include "MaterialFromMassFractions.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

IMaterial* MaterialFromMassFractions::create(const std::string& name, State state)
{
	return (new MaterialFromMassFractions(name,state));
}

MaterialFromMassFractions::MaterialFromMassFractions(const std::string& name) : IMaterial(name)
{
}

MaterialFromMassFractions::MaterialFromMassFractions(const std::string& name, State state) : IMaterial(name,state)
{
}

MaterialFromMassFractions::MaterialFromMassFractions(const std::string& name, const std::string& strState) : IMaterial(name,strState)
{
}

MaterialFromMassFractions::~MaterialFromMassFractions()
{
}

IMaterial::BuildingMode MaterialFromMassFractions::getBuildingMode() const
{
	return BuildingMode::MassFractions;
}

double MaterialFromMassFractions::getMassDensity() const
{
	return _massDensity;
}

void MaterialFromMassFractions::setMassDensity(double massDensity)
{
	if (massDensity<=0)
		throw SX::Kernel::Error<MaterialFromMassFractions>("Invalid density value.");

	_massDensity=massDensity;
}

void MaterialFromMassFractions::addElement(sptrElement element, double massFraction)
{
	if (massFraction<=0 || massFraction>1)
		throw SX::Kernel::Error<MaterialFromMassFractions>("Invalid value for mass fraction.");
	double sum=std::accumulate(std::begin(_contents),
								std::end(_contents),
								massFraction,
								[](double previous, const strToDoublePair& p) { return previous+p.second;});
	if (sum>(1.00000001))
		throw SX::Kernel::Error<MaterialFromMassFractions>("The sum of mass fractions exceeds 1.0");

	std::string eName=element->getName();

	auto it=_contents.find(eName);
	if (it!=_contents.end())
		it->second += massFraction;
	else
	{
		_elements.insert(strToElementPair(eName,element));
		_contents.insert(strToDoublePair(eName,massFraction));
	}

	return;
}

void MaterialFromMassFractions::addElement(const std::string& name, double massFraction)
{
	ElementManager* mgr=ElementManager::Instance();

	sptrElement el=mgr->getElement(name);

	addElement(el,massFraction);

	return;
}

void MaterialFromMassFractions::addMaterial(sptrMaterial material, double massFraction)
{

	for (const auto& p : material->getMassFractions())
		addElement(p.first,massFraction*p.second);

	//! If the material to add has a different chemical state then set the state as a mixture of chemical states
	if (_state != material->getState())
		_state=State::Unknown;
}

strToDoubleMap MaterialFromMassFractions::getMassFractions() const
{
	return _contents;
}

strToDoubleMap MaterialFromMassFractions::getMolarFractions() const
{
	strToDoubleMap molarFractions;

	double fact=0.0;
	for (auto p : _contents)
	{
		double fraction=p.second/_elements.at(p.first)->getMolarMass();
		fact+=fraction;
		molarFractions.insert(strToDoublePair(p.first,fraction));
	}
	for (auto& f : molarFractions)
		f.second/=fact;

	return molarFractions;
}

strToDoubleMap MaterialFromMassFractions::getPartialPressures() const
{
	if (_state!=State::Gaz)
		throw SX::Kernel::Error<MaterialFromMassFractions>("Invalid material state.");

	if (_temperature < 1.0-9)
		throw SX::Kernel::Error<MaterialFromMassFractions>("Invalid temperature.");

	if (_massDensity < 1.0-9)
		throw SX::Kernel::Error<MaterialFromMassFractions>("Invalid density.");

	strToDoubleMap partialPressures=getMolarFractions();
	double fact(0.0);
	for (const auto& p : partialPressures)
		fact+=p.second*_elements.at(p.first)->getMolarMass();
	double totalPressure(_massDensity*SX::Units::R/_temperature/fact);

	for (auto& f : partialPressures)
		f.second*=totalPressure;

	return partialPressures;
}

strToDoubleMap MaterialFromMassFractions::getStoichiometry() const
{
	strToDoubleMap stochiometry=getMolarFractions();

	auto it=std::min_element(stochiometry.begin(),
			                          stochiometry.end(),
			                          [] (const strToDoublePair & p1, const strToDoublePair & p2) -> bool {return (p1.second < p2.second);});

	double minFraction=it->second;

	for (auto& f : stochiometry)
		f.second/=minFraction;

	return stochiometry;
}

void MaterialFromMassFractions::writeToXML(property_tree::ptree& parent) const
{
	property_tree::ptree& node=parent.add("material","");
	node.put("<xmlattr>.name",_name);
	node.put("<xmlattr>.chemical_state",stateToStr.at(_state));
	node.put("<xmlattr>.building_mode","mass fractions");
	node.put<double>("mass_density",getMassDensity());

	for (const auto& el : getMassFractions())
	{
		property_tree::ptree& elnode=node.add("element","");
		elnode.put("<xmlattr>.name",el.first);
		elnode.put<double>("contents",el.second);
	}
}

void MaterialFromMassFractions::print(std::ostream& os) const
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
		os<<"Mass fractions:"<<std::endl;
		for (const auto& p : _elements)
		{
			os<<"\t-"<<std::setw(maxSize)<<std::setiosflags(std::ios::left)<<p.second->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<p.second<<std::endl;
			std::cout<<std::resetiosflags(std::ios::right);
		}
	}
}

}

} // end namespace SX
