#include <iostream>

#include "Element.h"
#include "Error.h"
#include "IMaterial.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

std::map<std::string,BuildingMode> IMaterial::strToBuildingMode={
		{"mass_fractions",BuildingMode::MassFractions},
		{"molar_fractions",BuildingMode::MolarFractions},
		{"stoichiometry",BuildingMode::Stoichiometry},
		{"partial_pressures",BuildingMode::PartialPressures}
};

std::map<BuildingMode,std::string> IMaterial::buildingModeToStr={
		{BuildingMode::MassFractions,"mass_fractions"},
		{BuildingMode::MolarFractions,"molar_fractions"},
		{BuildingMode::Stoichiometry,"stoichiometry"},
		{BuildingMode::PartialPressures,"partial_pressures"}
};

std::map<std::string,ChemicalState> IMaterial::strToState={
		{"solid",ChemicalState::Solid},
		{"liquid",ChemicalState::Liquid},
		{"gaz",ChemicalState::Gaz},
		{"unknown",ChemicalState::Unknown}};

std::map<ChemicalState,std::string> IMaterial::stateToStr={
		{ChemicalState::Solid,"solid"},
		{ChemicalState::Liquid,"liquid"},
		{ChemicalState::Gaz,"gaz"},
		{ChemicalState::Unknown,"unknown"}};

IMaterial::IMaterial(const std::string& name) : _name(name), _state(ChemicalState::Unknown), _massDensity(0.0), _temperature(0.0), _elements(), _contents()
{
}

IMaterial::IMaterial(const std::string& name, ChemicalState state) : _name(name), _state(state), _massDensity(0.0), _temperature(0.0), _elements(), _contents()
{
}

IMaterial::IMaterial(const std::string& name, const std::string& state) : _name(name), _state(strToState.at(state)), _massDensity(0.0), _temperature(0.0), _elements(), _contents()
{
}

IMaterial::~IMaterial()
{
}

bool IMaterial::operator==(const IMaterial& other) const
{

	strToDoubleMap mf1=getMassFractions();
	strToDoubleMap mf2=other.getMassFractions();

	return (_massDensity==other._massDensity) &&
			(_state==other._state) &&
			(mf1.size() == mf2.size()) &&
			std::equal(mf1.begin(),
					   mf1.end(),
					   mf2.begin(),
					   [] (strToDoublePair a, strToDoublePair b) { return a.first==b.first && std::abs(a.second-b.second)<1.0e-6;});
}

sptrElement IMaterial::operator[](const std::string& name)
{
	try
	{
		return _elements.at(name);
	}
	catch(const std::out_of_range& e)
	{
		throw SX::Kernel::Error<IMaterial>("No element match "+name+" name in material "+_name);
	}
}

const std::string& IMaterial::getName() const
{
	return _name;
}

unsigned int IMaterial::getNElements() const
{
	return _elements.size();
}

ChemicalState IMaterial::getState() const
{
	return _state;
}

std::string IMaterial::getStateString() const
{
	return stateToStr.at(_state);
}

double IMaterial::getTemperature() const
{
	return _temperature;
}

void IMaterial::setTemperature(double temperature)
{
	if (temperature < 1.0-9)
		throw SX::Kernel::Error<IMaterial>("Invalid temperature.");

	_temperature=temperature;
}

strToDoubleMap IMaterial::getNAtomsPerVolume() const
{
	strToDoubleMap nAtomsPerVolume;

	double fact=Units::avogadro*getMassDensity();

	auto massFractions=getMassFractions();

	for (const auto& p : massFractions)
	{
		double molarMass=_elements.at(p.first)->getMolarMass();
		nAtomsPerVolume.insert(strToDoublePair(p.first,fact*p.second/molarMass));
	}

	return nAtomsPerVolume;
}

strToDoubleMap IMaterial::getNElectronsPerVolume() const
{
	strToDoubleMap nElectronsPerVolume;

	double fact=Units::avogadro*getMassDensity();

	for (const auto& p : getMassFractions())
	{
		double nElectrons=static_cast<double>(_elements.at(p.first)->getNElectrons());
		double molarMass=static_cast<double>(_elements.at(p.first)->getMolarMass());
		nElectronsPerVolume.insert(strToDoublePair(p.first,static_cast<double>(fact*nElectrons)*p.second/molarMass));
	}

	return nElectronsPerVolume;
}

double IMaterial::getNAtomsTotalPerVolume() const
{
	auto nAtomsPerVolume=getNAtomsPerVolume();

	double nAtomsPerVolumeTotal = std::accumulate(std::begin(nAtomsPerVolume),
			                                       std::end(nAtomsPerVolume),
			                                       0.0,
			                                       [](double previous, const strToDoublePair& p){return previous+p.second;});
	return nAtomsPerVolumeTotal;
}

double IMaterial::getNElectronsTotalPerVolume() const
{
	auto nElectronsPerVolume=getNElectronsPerVolume();

	double nElectronsPerVolumeTotal=std::accumulate(std::begin(nElectronsPerVolume),
			                                         std::end(nElectronsPerVolume),
			                                         0.0,
			                                         [](double previous, const strToDoublePair& p){return previous+p.second;});
	return nElectronsPerVolumeTotal;
}

double IMaterial::getMuScattering() const
{
	double muScat=0.0;
	auto nAtomsPerVolume=getNAtomsPerVolume();
	for (const auto& p : nAtomsPerVolume)
	{
		double xsInc=_elements.at(p.first)->getIncoherentXs();
		muScat+=p.second*xsInc;
	}
	return muScat;
}

double IMaterial::getMuAbsorption(double lambda) const
{
	double muAbs=0.0;
	auto nAtomsPerVolume=getNAtomsPerVolume();
	for (const auto& p : nAtomsPerVolume)
	{
		double xsAbs=_elements.at(p.first)->getAbsorptionXs(lambda);
		muAbs+=p.second*xsAbs;
	}
	return muAbs;
}

double IMaterial::getMu(double lambda) const
{
	double mu=getMuScattering() + getMuAbsorption(lambda);
	return mu;
}

std::ostream& operator<<(std::ostream& os, const IMaterial& material)
{
	material.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
