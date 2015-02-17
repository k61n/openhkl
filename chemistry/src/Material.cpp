#include <algorithm>
#include <functional>
#include <iomanip>
#include <iterator>
#include <numeric>

#include "Error.h"
#include "Element.h"
#include "ElementManager.h"
#include "Material.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

std::map<std::string,Material::State> Material::s_toState={
		{"solid",Material::State::Solid},
		{"liquid",Material::State::Liquid},
		{"gaz",Material::State::Gaz}
};

std::map<Material::State,std::string> Material::s_fromState={
		{Material::State::Solid,"solid"},
		{Material::State::Liquid,"liquid"},
		{Material::State::Gaz,"gaz"}
};

std::map<std::string,Material::FillingMode> Material::s_toFillingMode={
		{"mass_fraction",Material::FillingMode::MassFraction},
		{"mole_fraction",Material::FillingMode::MoleFraction},
		{"number_of_atoms",Material::FillingMode::NumberOfAtoms},
		{"partial_pressure",Material::FillingMode::PartialPressure}
};

std::map<Material::FillingMode,std::string> Material::s_fromFillingMode={
		{Material::FillingMode::MassFraction,"mass_fraction"},
		{Material::FillingMode::MoleFraction,"mole_fraction"},
		{Material::FillingMode::NumberOfAtoms,"number_of_atoms"},
		{Material::FillingMode::PartialPressure,"partial_pressure"}
};

Material* Material::create(const std::string& name, State state, FillingMode fillingMode)
{
	Material* material=new Material(name,state,fillingMode);
	return material;
}

Material::Material(const std::string& name, State state, FillingMode fillingMode)
: _name(name),
  _density(0.0),
  _temperature(0.0),
  _state(state),
  _fillingMode(fillingMode),
  _elements()
{
}

Material::~Material()
{
}

bool Material::operator==(const Material& other) const
{

	elementContentsMap mf1=getMassFractions();
	elementContentsMap mf2=other.getMassFractions();

	return (_density==other._density) &&
			(_state==other._state) &&
			(mf1.size() == mf2.size()) &&
			std::equal(mf1.begin(),
					   mf1.end(),
					   mf2.begin(),
					   [] (elementContentsPair a, elementContentsPair b) { return a.first==b.first && std::abs(a.second-b.second)<1.0e-6;});
}

void Material::addElement(Element* element, double fraction)
{

	if (_fillingMode==FillingMode::MassFraction || _fillingMode==FillingMode::MoleFraction)
	{
		if (fraction<=0 || fraction>1)
			throw SX::Kernel::Error<Material>("Invalid value for mass/mole fraction");
		double sum=std::accumulate(std::begin(_elements),
				                    std::end(_elements),
				                    fraction,
				                    [](double previous, const elementContentsPair& p) { return previous+p.second;});
		if (sum>(1.000001))
			throw SX::Kernel::Error<Material>("The sum of mole fractions exceeds 1.0");
	}

	auto it=_elements.find(element);
	if (it!=_elements.end())
		it->second += fraction;
	else
		_elements.insert(elementContentsPair(element,fraction));

	return;
}

void Material::addElement(const std::string& name, double fraction)
{
	ElementManager* mgr=ElementManager::Instance();

	Element* el=mgr->findElement(name);

	addElement(el,fraction);

	return;
}

void Material::addMaterial(Material* material, double contents)
{
	if (_fillingMode==FillingMode::MassFraction)
	{
		for (auto it : material->getMassFractions())
			addElement(it.first,contents*it.second);
	}
	else if (_fillingMode==FillingMode::MoleFraction || _fillingMode==FillingMode::PartialPressure)
	{
		for (auto it : material->getMoleFractions())
			addElement(it.first,contents*it.second);
	}
	else
		throw SX::Kernel::Error<Material>("Invalid filling mode");
}
//
//void Material::addMaterial(const std::string& name, double contents)
//{
//	Material* mat=Material::buildFromDatabase(name);
//
//	addMaterial(mat,contents);
//
//	return;
//}
//
elementContentsMap Material::getMassFractions() const
{

	elementContentsMap fractions;

	switch(_fillingMode)
	{
	case FillingMode::MassFraction:
	{
		fractions = _elements;
		break;
	}

	case FillingMode::MoleFraction:
	{
		double fact=0.0;
		for (auto it=_elements.begin();it!=_elements.end();++it)
		{
			fact+=it->second*it->first->getMolarMass();
			fractions.insert(std::pair<Element*,double>(it->first,it->second*it->first->getMolarMass()));
		}
		for (auto& f : fractions)
			f.second/=fact;
		break;
	}

	case FillingMode::NumberOfAtoms:
	{
		double totalMass=0.0;
		for (auto it=_elements.begin();it!=_elements.end();++it)
		{
			double fimi=it->first->getMolarMass()*it->second;
			fractions.insert(elementContentsPair(it->first,fimi));
			totalMass+=fimi;
		}
		for (auto& it : fractions)
			it.second/=totalMass;
		break;
	}

	case FillingMode::PartialPressure:
	{
		double fact=0.0;
		for (auto it=_elements.begin();it!=_elements.end();++it)
		{
			fact+=it->second*it->first->getMolarMass();
			fractions.insert(std::pair<Element*,double>(it->first,it->second*it->first->getMolarMass()));
		}
		for (auto& f : fractions)
			f.second/=fact;
		break;
	}

	}

	return fractions;
}

elementContentsMap Material::getMoleFractions() const
{

	elementContentsMap fractions;

	switch(_fillingMode)
	{

	case FillingMode::MassFraction:
	{
		double fact=0.0;
		for (auto it=_elements.begin();it!=_elements.end();++it)
		{
			fact+=it->second/it->first->getMolarMass();
			fractions.insert(std::pair<Element*,double>(it->first,it->second/it->first->getMolarMass()));
		}
		for (auto& f : fractions)
			f.second/=fact;
		break;
	}

	case FillingMode::MoleFraction:
	{
		fractions = _elements;
		break;
	}

	case FillingMode::NumberOfAtoms:
	{
		double nAtoms=std::accumulate(std::begin(_elements),
				                       std::end(_elements),
				                       0.0,
				                       [](double previous, const elementContentsPair& p) {return previous+p.second;});
		for (auto it=_elements.begin();it!=_elements.end();++it)
			fractions.insert(std::pair<Element*,double>(it->first,it->second/nAtoms));
		break;
	}

	case FillingMode::PartialPressure:
	{
		double totalPressure=std::accumulate(std::begin(_elements),
				                              std::end(_elements),
				                              0.0,
				                              [](double previous, const elementContentsPair& p) {return previous+p.second;});
		for (auto it=_elements.begin();it!=_elements.end();++it)
			fractions.insert(std::pair<Element*,double>(it->first,it->second/totalPressure));
		break;
	}

	}

	return fractions;
}

elementContentsMap Material::getNAtomsPerVolume() const
{
	elementContentsMap nAtoms;

	double fact=SX::Units::avogadro*getDensity();

	auto massFractions=getMassFractions();

	for (auto it : massFractions)
		nAtoms.insert(elementContentsPair(it.first,fact*it.second/it.first->getMolarMass()));

	return nAtoms;
}

double Material::getNAtomsTotalPerVolume() const
{
	elementContentsMap nAtoms=getNAtomsPerVolume();

	return std::accumulate(std::begin(nAtoms),
			                std::end(nAtoms),
			                0.0,
			                [](double previous, const elementContentsPair& p){return previous+p.second;});
}

elementContentsMap Material::getNElectronsPerVolume() const
{
	elementContentsMap nAtoms;

	double fact=SX::Units::avogadro*getDensity();

	for (auto it : getMassFractions())
		nAtoms.insert(elementContentsPair(it.first,static_cast<double>(fact*it.first->getNElectrons())*it.second/it.first->getMolarMass()));

	return nAtoms;
}

double Material::getNElectronsTotalPerVolume() const
{
	elementContentsMap nElectrons=getNElectronsPerVolume();

	return std::accumulate(std::begin(nElectrons),
			                std::end(nElectrons),
			                0.0,
			                [](double previous, const elementContentsPair& p){return previous+p.second;});
}

double Material::getMu(double lambda) const
{
	double mu=0.0;
	auto nAtomsPerVol=getNAtomsPerVolume();
	for (auto it : nAtomsPerVol)
	{
		double xs=it.first->getIncoherentXs() + it.first->getAbsorptionXs(lambda);
		mu+=it.second*xs;
	}
	return mu;
}

const std::string& Material::getName() const
{
	return _name;
}

unsigned int Material::getNElements() const
{
	return _elements.size();
}

double Material::getDensity() const
{
	if (_fillingMode==FillingMode::PartialPressure)
	{
		if (_temperature<=0)
			throw SX::Kernel::Error<Material>("Invalid temperature value");
		double totalPressure=std::accumulate(std::begin(_elements),
				                              std::end(_elements),
				                              0.0,
				                              [](double previous, const elementContentsPair& p) {return previous+p.second;});

		double moleDensity=totalPressure/SX::Units::R/_temperature;

		double density=0.0;

		elementContentsMap moleFractions=getMoleFractions();

		for (auto it : moleFractions)
			density+=moleDensity*it.second*it.first->getMolarMass();

		return density;

	}
	else
		return _density;
}

void Material::setDensity(double density)
{
	if (_fillingMode==FillingMode::PartialPressure)
		throw SX::Kernel::Error<Material>("The material has been filled with partial pressures. Its density will be directly computed from them.");

	if (density<=0)
		throw SX::Kernel::Error<Material>("Invalid density value: must be a strictly positive number");

	_density=density;
}

double Material::getTemperature() const
{
	return _temperature;
}

void Material::setTemperature(double temperature)
{
	if (temperature<=0)
		throw SX::Kernel::Error<Material>("Negative temperature value");

	_temperature=temperature;
}

void Material::print(std::ostream& os) const
{
	os<<"Material "<<_name<<" --> State="<<s_fromState[_state]<<" ; Filling mode="<<s_fromFillingMode[_fillingMode]<<std::endl;
	if (_elements.empty())
		os<<"Currently empty"<<std::endl;
	else
	{
		unsigned int maxSize=0;
		for (auto it : _elements)
			if (it.first->getName().size() > maxSize)
				maxSize=it.first->getName().size();
		os<<"Composition:"<<std::endl;
		for (auto it : _elements)
		{
			os<<"\t-"<<std::setw(maxSize)<<std::setiosflags(std::ios::left)<<it.first->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<it.second<<std::endl;
			std::cout<<std::resetiosflags(std::ios::right);
		}
	}
}

Material::FillingMode Material::getFillingMode() const
{
	return _fillingMode;
}

Material::State Material::getState() const
{
	return _state;
}

std::ostream& operator<<(std::ostream& os, const Material& material)
{
	material.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
