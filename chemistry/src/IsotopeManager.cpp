#include "Isotope.h"
#include "IsotopeManager.h"
#include "Units.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

isotopeMap IsotopeManager::_isotopeRegistry=isotopeMap();

std::string IsotopeManager::database="SXIsotopeDatabase.xml";

IsotopeManager::IsotopeManager()
{
}

unsigned int IsotopeManager::getNRegisteredIsotopes() const
{
	return _isotopeRegistry.size();
}

Isotope* IsotopeManager::getIsotope(const std::string& name) const
{
	auto p=_isotopeRegistry.find(name);

	if (p!=_isotopeRegistry.end())
		return p->second;
	else
	{
		ptree root;
		read_xml(database,root);

		BOOST_FOREACH(ptree::value_type const& v, root.get_child("sx_isotope_database"))
		{
			if (v.first.compare("isotope")!=0)
				continue;

			if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
			{
				Isotope* is=readIsotope(v.second);
				auto ret=_isotopeRegistry.insert(isotopePair(name,is));
				return ret.first->second;
			}
		}
		throw SX::Kernel::Error<IsotopeManager>("No match for entry "+name);
	}
}

Isotope* IsotopeManager::readIsotope(const ptree& node) const
{
	SX::Units::UnitsManager* um = SX::Units::UnitsManager::Instance();
	double units;

	Isotope* is=new Isotope();
	is->_name=node.get<std::string>("<xmlattr>.name");

	is->_symbol=node.get<std::string>("symbol");

	is->_element=node.get<std::string>("element");

	is->_nProtons=node.get<int>("nProtons");

	is->_nNucleons=node.get<int>("nNucleons");

	is->_nElectrons=is->_nProtons;

	units=um->get(node.get<std::string>("molarMass.<xmlattr>.units","uma"));
	is->_molarMass=node.get<double>("molarMass")*units;

	is->_nuclearSpin=node.get<double>("nuclearSpin");

	is->_state=node.get<std::string>("state");

	units=um->get(node.get<std::string>("abundance.<xmlattr>.units","%"));
	is->_abundance=node.get<double>("abundance",0.0)*units;

	units=um->get(node.get<std::string>("halfLife.<xmlattr>.units","year"));
	is->_halfLife=node.get<double>("halfLife",std::numeric_limits<double>::infinity())*units;

	is->_stable=node.get<bool>("stable");

	units=um->get(node.get<std::string>("bCoherent.<xmlattr>.units","fm"));
	is->_bCoherent=node.get<std::complex<double>>("bCoherent")*units;

	units=um->get(node.get<std::string>("bIncoherent.<xmlattr>.units","fm"));
	is->_bIncoherent=node.get<std::complex<double>>("bIncoherent")*units;

	units=um->get(node.get<std::string>("bPlus.<xmlattr>.units","fm"));
	is->_bPlus=node.get<std::complex<double>>("bPlus")*units;

	units=um->get(node.get<std::string>("bMinus.<xmlattr>.units","fm"));
	is->_bMinus=node.get<std::complex<double>>("bMinus")*units;

	units=um->get(node.get<std::string>("xsCoherent.<xmlattr>.units","barn"));
	is->_xsCoherent=node.get<double>("xsCoherent")*units;

	units=um->get(node.get<std::string>("xsIncoherent.<xmlattr>.units","barn"));
	is->_xsIncoherent=node.get<double>("xsIncoherent")*units;

	units=um->get(node.get<std::string>("xsScattering.<xmlattr>.units","barn"));
	is->_xsScattering=node.get<double>("xsScattering")*units;

	units=um->get(node.get<std::string>("xsAbsorption.<xmlattr>.units","barn"));
	is->_xsAbsorption=node.get<double>("xsAbsorption")*units;

	return is;
}

} // end namespace Chemistry

} // end namespace SX
