#include <limits>

#include "Isotope.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

isotopeMap Isotope::registry=isotopeMap();

std::string Isotope::database="isotopes.xml";

Isotope* Isotope::buildFromDatabase(const std::string& name)
{
	auto it=registry.find(name);
	if (it!=registry.end())
		return it->second;
	else
	{
		ptree root;
		read_xml(database,root);

		BOOST_FOREACH(ptree::value_type const& v, root.get_child("isotopes"))
		{
			if (v.first.compare("isotope")!=0)
				continue;

			if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
				return readIsotope(v.second);
		}
	}
	throw SX::Kernel::Error<Isotope>("Isotope "+name+" is not registered in the isotopes database");
}

unsigned int Isotope::getNRegisteredIsotopes()
{
	return registry.size();
}

Isotope* Isotope::readIsotope(const ptree& node)
{

	std::string name=node.get<std::string>("<xmlattr>.name");
	auto it=registry.find(name);
	if (it!=registry.end())
		return it->second;

	SX::Units::UnitsManager* um = SX::Units::UnitsManager::Instance();
	double units;

	Isotope* is=new Isotope();
	is->_name=name;

	is->_symbol=node.get<std::string>("symbol");

	is->_element=node.get<std::string>("element");

	is->_nProtons=node.get<int>("n_protons");

	is->_nNucleons=node.get<int>("n_nucleons");

	is->_nElectrons=is->_nProtons;

	units=um->get(node.get<std::string>("molarMass.<xmlattr>.units","uma"));
	is->_molarMass=node.get<double>("molar_mass")*units;

	is->_nuclearSpin=node.get<double>("nuclear_spin");

	is->_state=node.get<std::string>("state");

	units=um->get(node.get<std::string>("half_life.<xmlattr>.units","%"));
	is->_abundance=node.get<double>("abundance",0.0)*units;

	units=um->get(node.get<std::string>("halfLife.<xmlattr>.units","year"));
	is->_halfLife=node.get<double>("half_life",std::numeric_limits<double>::infinity())*units;

	is->_stable=node.get<bool>("stable");

	units=um->get(node.get<std::string>("b_coherent.<xmlattr>.units","fm"));
	is->_bCoherent=node.get<std::complex<double>>("b_coherent")*units;

	units=um->get(node.get<std::string>("b_incoherent.<xmlattr>.units","fm"));
	is->_bIncoherent=node.get<std::complex<double>>("b_incoherent")*units;

	units=um->get(node.get<std::string>("b_plus.<xmlattr>.units","fm"));
	is->_bPlus=node.get<std::complex<double>>("b_plus",is->_bCoherent)*units;

	units=um->get(node.get<std::string>("b_minus.<xmlattr>.units","fm"));
	is->_bMinus=node.get<std::complex<double>>("b_minus",is->_bCoherent)*units;

	units=um->get(node.get<std::string>("xs_coherent.<xmlattr>.units","barn"));
	is->_xsCoherent=node.get<double>("xs_coherent")*units;

	units=um->get(node.get<std::string>("xs_incoherent.<xmlattr>.units","barn"));
	is->_xsIncoherent=node.get<double>("xs_incoherent")*units;

	units=um->get(node.get<std::string>("xs_scattering.<xmlattr>.units","barn"));
	is->_xsScattering=node.get<double>("xs_scattering")*units;

	units=um->get(node.get<std::string>("xs_absorption.<xmlattr>.units","barn"));
	is->_xsAbsorption=node.get<double>("xs_absorption")*units;

	registry.insert(isotopePair(is->_name,is));

	return is;
}

Isotope::Isotope()
: _name(""),
  _symbol(""),
  _element(""),
  _nProtons(0),
  _nNucleons(0),
  _nElectrons(0),
  _molarMass(0.0),
  _nuclearSpin(0.0),
  _state(""),
  _abundance(0.0),
  _halfLife(0.0),
  _stable(true),
  _bCoherent(0.0),
  _bIncoherent(0.0),
  _bPlus(0.0),
  _bMinus(0.0),
  _xsCoherent(0.0),
  _xsIncoherent(0.0),
  _xsScattering(0.0),
  _xsAbsorption(0.0)
{
}

Isotope::~Isotope()
{
}

bool Isotope::operator==(const Isotope& other)
{
	return ((_nProtons==other._nProtons) && (_nNucleons==other._nNucleons));
}

const std::string& Isotope::getName() const
{
	return _name;
}

double Isotope::getAbundance() const
{
	return _abundance;
}

double Isotope::getFormalCharge() const
{
	return (_nProtons-_nElectrons);
}

double Isotope::getMolarMass() const
{
	return _molarMass;
}

double Isotope::getNNeutrons() const
{
	return (_nNucleons-_nProtons);
}

double Isotope::getNNucleons() const
{
	return _nNucleons;
}

double Isotope::getNElectrons() const
{
	return _nElectrons;
}

double Isotope::getNProtons() const
{
	return _nProtons;
}

bool Isotope::isIon() const
{
	return (_nProtons!=_nElectrons);
}

bool Isotope::isAnion() const
{
	return (_nProtons<_nElectrons);
}

bool Isotope::isCation() const
{
	return (_nProtons>_nElectrons);
}

void Isotope::print(std::ostream& os) const
{
	os<<"Isotope "<<_name<<" ["<<_nProtons<<","<<_nNucleons<<"]";
}

std::ostream& operator<<(std::ostream& os,const Isotope& isotope)
{
	isotope.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
