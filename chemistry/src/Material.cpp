#include <algorithm>
#include <functional>
#include <iomanip>
#include <iterator>
#include <numeric>
#include <stdexcept>

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
  _elements(),
  _contents()
{
}

Material::~Material()
{
}

bool Material::operator==(const Material& other) const
{

	contentsMap mf1=getMassFractions();
	contentsMap mf2=other.getMassFractions();

	return (_density==other._density) &&
			(_state==other._state) &&
			(mf1.size() == mf2.size()) &&
			std::equal(mf1.begin(),
					   mf1.end(),
					   mf2.begin(),
					   [] (strToDoublePair a, strToDoublePair b) { return a.first==b.first && std::abs(a.second-b.second)<1.0e-6;});
}

Element* Material::operator[](const std::string& name)
{
	try
	{
		return _elements.at(name);
	}
	catch(const std::out_of_range& e)
	{
		throw SX::Kernel::Error<Material>("No element match "+name+" name in material "+_name);
	}
}

void Material::addElement(Element* element, double fraction)
{

	if (_fillingMode==FillingMode::MassFraction || _fillingMode==FillingMode::MoleFraction)
	{
		if (fraction<=0 || fraction>1)
			throw SX::Kernel::Error<Material>("Invalid value for mass/mole fraction");
		double sum=std::accumulate(std::begin(_contents),
				                    std::end(_contents),
				                    fraction,
				                    [](double previous, const strToDoublePair& p) { return previous+p.second;});
		if (sum>(1.000001))
			throw SX::Kernel::Error<Material>("The sum of mole fractions exceeds 1.0");
	}

	std::string eName=element->getName();

	auto it=_contents.find(eName);
	if (it!=_contents.end())
		it->second += fraction;
	else
	{
		_elements.insert(strToElementPair(eName,element));
		_contents.insert(strToDoublePair(eName,fraction));
	}

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
		for (const auto& p : material->getMassFractions())
			addElement(p.first,contents*p.second);
	}
	else if (_fillingMode==FillingMode::MoleFraction || _fillingMode==FillingMode::PartialPressure)
	{
		for (const auto& p : material->getMoleFractions())
			addElement(p.first,contents*p.second);
	}
	else
		throw SX::Kernel::Error<Material>("Invalid filling mode");
}

contentsMap Material::getMassFractions() const
{

	contentsMap fractions;

	switch(_fillingMode)
	{
	case FillingMode::MassFraction:
	{
		fractions = _contents;
		break;
	}

	case FillingMode::MoleFraction:
	{
		double fact=0.0;
		for (auto it=_contents.begin();it!=_contents.end();++it)
		{
			double prod=it->second*_elements.at(it->first)->getMolarMass();
			fact+=prod;
			fractions.insert(strToDoublePair(it->first,prod));
		}
		for (auto& f : fractions)
			f.second/=fact;
		break;
	}

	case FillingMode::NumberOfAtoms:
	{
		double totalMass=0.0;
		for (auto it=_contents.begin();it!=_contents.end();++it)
		{
			double prod=it->second*_elements.at(it->first)->getMolarMass();
			totalMass+=prod;
			fractions.insert(strToDoublePair(it->first,prod));
		}
		for (auto& it : fractions)
			it.second/=totalMass;
		break;
	}

	case FillingMode::PartialPressure:
	{
		double fact=0.0;
		for (auto it=_contents.begin();it!=_contents.end();++it)
		{
			double prod=it->second*_elements.at(it->first)->getMolarMass();
			fact+=prod;
			fractions.insert(strToDoublePair(it->first,prod));
		}
		for (auto& f : fractions)
			f.second/=fact;
		break;
	}

	}

	return fractions;
}

contentsMap Material::getMoleFractions() const
{

	contentsMap fractions;

	switch(_fillingMode)
	{

	case FillingMode::MassFraction:
	{
		double fact=0.0;
		for (auto it=_contents.begin();it!=_contents.end();++it)
		{
			double frac=it->second/_elements.at(it->first)->getMolarMass();
			fact+=frac;
			fractions.insert(strToDoublePair(it->first,frac));
		}
		for (auto& f : fractions)
			f.second/=fact;
		break;
	}

	case FillingMode::MoleFraction:
	{
		fractions = _contents;
		break;
	}

	case FillingMode::NumberOfAtoms:
	{
		double nAtoms=std::accumulate(std::begin(_contents),
				                       std::end(_contents),
				                       0.0,
				                       [](double previous, const strToDoublePair& p) {return previous+p.second;});
		for (auto it=_contents.begin();it!=_contents.end();++it)
			fractions.insert(strToDoublePair(it->first,it->second/nAtoms));
		break;
	}

	case FillingMode::PartialPressure:
	{
		double totalPressure=std::accumulate(std::begin(_contents),
				                              std::end(_contents),
				                              0.0,
				                              [](double previous, const strToDoublePair& p) {return previous+p.second;});
		for (auto it=_contents.begin();it!=_contents.end();++it)
			fractions.insert(strToDoublePair(it->first,it->second/totalPressure));
		break;
	}

	}

	return fractions;
}

contentsMap Material::getNAtomsPerVolume() const
{
	contentsMap nAtoms;

	double fact=SX::Units::avogadro*getDensity();

	auto massFractions=getMassFractions();

	for (const auto& p : massFractions)
	{
		double molarMass=_elements.at(p.first)->getMolarMass();
		nAtoms.insert(strToDoublePair(p.first,fact*p.second/molarMass));
	}

	return nAtoms;
}

double Material::getNAtomsTotalPerVolume() const
{
	contentsMap nAtoms=getNAtomsPerVolume();

	return std::accumulate(std::begin(nAtoms),
			                std::end(nAtoms),
			                0.0,
			                [](double previous, const strToDoublePair& p){return previous+p.second;});
}

contentsMap Material::getNElectronsPerVolume() const
{
	contentsMap nAtoms;

	double fact=SX::Units::avogadro*getDensity();

	for (const auto& p : getMassFractions())
	{
		double nElectrons=static_cast<double>(_elements.at(p.first)->getNElectrons());
		double molarMass=static_cast<double>(_elements.at(p.first)->getMolarMass());
		nAtoms.insert(strToDoublePair(p.first,static_cast<double>(fact*nElectrons)*p.second/molarMass));
	}

	return nAtoms;
}

double Material::getNElectronsTotalPerVolume() const
{
	contentsMap nElectrons=getNElectronsPerVolume();

	return std::accumulate(std::begin(nElectrons),
			                std::end(nElectrons),
			                0.0,
			                [](double previous, const strToDoublePair& p){return previous+p.second;});
}

double Material::getMu(double lambda) const
{
	double mu=0.0;
	auto nAtomsPerVol=getNAtomsPerVolume();
	for (const auto& p : nAtomsPerVol)
	{
		double xsInc=_elements.at(p.first)->getIncoherentXs();
		double xsAbs=_elements.at(p.first)->getAbsorptionXs(lambda);
		mu+=p.second*(xsInc + xsAbs);
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
		double totalPressure=std::accumulate(std::begin(_contents),
				                              std::end(_contents),
				                              0.0,
				                              [](double previous, const strToDoublePair& p) {return previous+p.second;});

		double moleDensity=totalPressure/SX::Units::R/_temperature;

		double density=0.0;

		contentsMap moleFractions=getMoleFractions();

		for (const auto& p : moleFractions)
			density+=moleDensity*p.second*static_cast<double>(_elements.at(p.first)->getMolarMass());

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
		for (const auto& p : _elements)
		{
			unsigned int nameSize=p.second->getName().size();
			if (nameSize > maxSize)
				maxSize=nameSize;
		}
		os<<"Composition:"<<std::endl;
		for (const auto& p : _elements)
		{
			os<<"\t-"<<std::setw(maxSize)<<std::setiosflags(std::ios::left)<<p.second->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<p.second<<std::endl;
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
