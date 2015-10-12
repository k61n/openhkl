#include <iostream>

#include <boost/filesystem.hpp>

#include "Error.h"
#include "Isotope.h"
#include "IsotopeManager.h"
#include "Path.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

namespace filesystem=boost::filesystem;

IsotopeManager::IsotopeManager() : _registry(), _units()
{
	// The default path for the isotopes database is $HOME/.nsxtool/databases/isotopes.xml
	filesystem::path p(SX::Utils::Path::getDataBasesPath());
	p/="isotopes.xml";

	setDatabasePath(p.string());
}

IsotopeManager::~IsotopeManager()
{
	cleanRegistry();
}

void IsotopeManager::cleanRegistry()
{
	for (auto it=_registry.begin();it!=_registry.end();)
	{
		if (it->second.unique())
			it=_registry.erase(it);
		else
			++it;
	}
}

const unitsMap& IsotopeManager::getUnits() const
{
	return _units;
}

void IsotopeManager::setDatabasePath(const std::string& path)
{
	// The new path is the same than the old path, do nothing
	if (path.compare(_database)==0)
		return;

	// The given path does not exists, throws
	if (!filesystem::exists(path))
		throw SX::Kernel::Error<IsotopeManager>("Invalid path for isotopes database.");

	// Loads the database into a property tree
	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(path,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<IsotopeManager>(error.what());
	}

	// Reads the units
	_units.clear();
	BOOST_FOREACH(const property_tree::ptree::value_type& v, root.get_child("units"))
	{
		_units.insert(unitsPair(v.first,v.second.get_value<std::string>()));
	}

	_database=path;
}

const std::string& IsotopeManager::getDatabasePath() const
{
	return _database;
}

const strToIsotopeMap& IsotopeManager::getRegistry() const
{
	return _registry;
}

std::vector<std::string> IsotopeManager::getDatabaseNames() const
{
	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<IsotopeManager>(error.what());
	}

	std::vector<std::string> names;
	names.reserve(200);

	BOOST_FOREACH(const property_tree::ptree::value_type& node, root.get_child("isotopes"))
	{
		if (node.first.compare("isotope")!=0)
			continue;
		names.push_back(node.second.get<std::string>("<xmlattr>.name"));
	}

	return names;
}

bool IsotopeManager::isInDatabase(const std::string& name) const
{
	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<IsotopeManager>(error.what());
	}

	BOOST_FOREACH(const property_tree::ptree::value_type& node, root.get_child("isotopes"))
	{
		if (node.first.compare("isotope")!=0)
			continue;

		if (node.second.get<std::string>("<xmlattr>.name").compare(name)==0)
			return true;
	}

	return false;
}

sptrIsotope IsotopeManager::buildIsotope(const property_tree::ptree& node)
{
	std::string name=node.get<std::string>("<xmlattr>.name");

	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	sptrIsotope isotope(Isotope::create(name));

	isotope->_symbol=node.get<std::string>("symbol");
	isotope->_element=node.get<std::string>("element");
	isotope->_nProtons=node.get<int>("n_protons");
	isotope->_nNucleons=node.get<int>("n_nucleons");
	isotope->_nElectrons=isotope->_nProtons;
	isotope->_molarMass=node.get<double>("molar_mass")*um->get(_units.at("molar_mass"));
	isotope->_nuclearSpin=node.get<double>("nuclear_spin");
	isotope->_chemicalState=node.get<std::string>("chemical_state");
	isotope->_naturalAbundance=node.get<double>("natural_abundance",0.0)*um->get(_units.at("natural_abundance"));
	isotope->_halfLife=node.get<double>("half_life",std::numeric_limits<double>::infinity())*um->get(_units.at("half_life"));
	isotope->_stable=node.get<bool>("stable");
	isotope->_bCoherent=node.get<std::complex<double>>("b_coherent")*um->get(_units.at("b_coherent"));
	isotope->_bIncoherent=node.get<std::complex<double>>("b_incoherent")*um->get(_units.at("b_incoherent"));
	isotope->_bPlus=node.get<std::complex<double>>("b_plus",isotope->_bCoherent)*um->get(_units.at("b_plus"));
	isotope->_bMinus=node.get<std::complex<double>>("b_minus",isotope->_bCoherent)*um->get(_units.at("b_minus"));
	isotope->_xsCoherent=node.get<double>("xs_coherent")*um->get(_units.at("xs_coherent"));
	isotope->_xsIncoherent=node.get<double>("xs_incoherent")*um->get(_units.at("xs_incoherent"));
	isotope->_xsScattering=node.get<double>("xs_scattering")*um->get(_units.at("xs_scattering"));
	isotope->_xsAbsorption=node.get<double>("xs_absorption")*um->get(_units.at("xs_absorption"));

	return isotope;

}

sptrIsotope IsotopeManager::getIsotope(const std::string& name)
{
	// If the isotope has already been registered, just return its corresponding pointer
	auto it=_registry.find(name);
	if (it!=_registry.end())
		return it->second;

	// Otherwise, build it from the XML database

	// Loads the database into a property tree
	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<IsotopeManager>(error.what());
	}

	BOOST_FOREACH(const property_tree::ptree::value_type& v, root.get_child("isotopes"))
	{
		if (v.first.compare("isotope")!=0)
			continue;

		if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
		{
			sptrIsotope isotope(buildIsotope(v.second));
			_registry.insert(strToIsotopePair(name,isotope));
			return isotope;
		}
	}

	// No isotope found neither in the registry neither in the database, throws
	throw Kernel::Error<IsotopeManager>("No isotope with "+name+" found neither in the registry, neither in the database.");
}

unsigned int IsotopeManager::getNIsotopesInRegistry() const
{
	return _registry.size();
}

unsigned int IsotopeManager::getNIsotopesInDatabase() const
{
	unsigned int nIsotopes(0);

	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<IsotopeManager>(error.what());
	}

	BOOST_FOREACH(const property_tree::ptree::value_type& node, root.get_child("isotopes"))
	{
		if (node.first.compare("isotope")!=0)
			continue;
		nIsotopes++;
	}

	return nIsotopes;
}

bool IsotopeManager::isInRegistry(const std::string& name) const
{
	auto it=_registry.find(name);
	return (it!=_registry.end());
}

} // end namespace Chemistry

} // end namespace SX
