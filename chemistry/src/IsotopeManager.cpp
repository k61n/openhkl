#include <iostream>

#include <boost/filesystem.hpp>

#include "Error.h"
#include "Isotope.h"
#include "IsotopeManager.h"
#include "Path.h"

namespace SX
{

namespace Chemistry
{

namespace filesystem=boost::filesystem;

IsotopeManager::IsotopeManager() : _registry()
{
	// The default path for the isotopes database is $HOME/.nsxtool/databases/isotopes.xml
	filesystem::path p(SX::Utils::Path::getApplicationDataPath());
	p/="databases";
	p/="isotopes.xml";
	_database = p.string();
}

IsotopeManager::~IsotopeManager()
{
	cleanRegistry();
}

void IsotopeManager::cleanRegistry()
{
	for (auto& p : _registry)
		delete p.second;

	_registry.clear();
}

void IsotopeManager::setDatabasePath(const std::string& path)
{
	// The new path is the same than the old path, do nothing
	if (path.compare(_database)==0)
		return;

	// The given path does not exists, throws
	if (!filesystem::exists(path))
		throw SX::Kernel::Error<IsotopeManager>("Invalid path for isotopes database.");

	_database=path;
}

Isotope* IsotopeManager::findIsotope(const std::string& name)
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
			// Once the XML node has been found, build an Isotope from it and register it
			Isotope* is = Isotope::create(v.second);
			_registry.insert(isotopePair(name,is));
			return is;
		}
	}

	// No node match the request, throws
	throw SX::Kernel::Error<IsotopeManager>("Isotope "+name+" is not registered in the isotopes database");
}

unsigned int IsotopeManager::getNRegisteredIsotopes() const
{
	return _registry.size();
}

bool IsotopeManager::hasIsotope(const std::string& name) const
{
	auto it=_registry.find(name);
	return (it!=_registry.end());
}

} // end namespace Chemistry

} // end namespace SX
