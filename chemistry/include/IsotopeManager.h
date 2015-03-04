/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_ISOTOPEMANAGER_H_
#define NSXTOOL_ISOTOPEMANAGER_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Isotope.h"
#include "Singleton.h"

namespace SX
{

namespace Chemistry
{

// Namespaces
namespace property_tree=boost::property_tree;
namespace xml_parser=boost::property_tree::xml_parser;

// Typedefs
typedef std::shared_ptr<Isotope> sptrIsotope;
typedef std::set<sptrIsotope> isotopeSet;

typedef std::map<std::string,sptrIsotope> isotopeMap;
typedef std::pair<std::string,sptrIsotope> isotopePair;

typedef std::map<std::string,std::string> unitsMap;
typedef std::pair<std::string,std::string> unitsPair;

class IsotopeManager : public SX::Kernel::Singleton<IsotopeManager,SX::Kernel::Constructor,SX::Kernel::Destructor>
{

public:

	//! Default constructor
	IsotopeManager();

	//! Destructor
	~IsotopeManager();

	//! Returns a set of shared pointer to the Isotope objects whose property |prop| matches the value |value|
	template<typename T>
	isotopeSet getIsotopes(const std::string& prop, T value);

	//! Returns the number of registered Isotope objects
	unsigned int getNIsotopesInRegistry() const;
	//! Returns true if an isotope with a given name is registered
	bool isInRegistry(const std::string& name) const;
	//! Cleans up the registry
	void cleanRegistry();

	//! Returns the value of a given property for a given isotope
	template<typename T>
	T getProperty(const std::string& name, const std::string& prop) const;

	//! Returns the name of the isotopes stored in the isotopes XML database
	std::vector<std::string> getDatabaseNames() const;
	//! Gets the path to the isotopes XML database
	const std::string& getDatabasePath() const;
	//! Returns the number of isotopes stored in the XML database
	unsigned int getNIsotopesInDatabase() const;
	//! Gets the units stored in the XML database
	const unitsMap& getUnits() const;
	//! Returns true if an isotope with a given name is stored in the XML database
	bool isInDatabase(const std::string& name) const;
	//! Sets the path for the isotopes XML database
	void setDatabasePath(const std::string& path);

	//! Returns a shared pointer to an Isotope with a given name. The Isotope is searched first in the registry then in the XML database. If not found return an empty isotope.
	sptrIsotope getIsotope(const std::string& name);

private:

	//! Builds and registers an Isotope from an XML node
	//! A shared pointer to the newly created Isotope is returned.
	sptrIsotope buildIsotope(const property_tree::ptree& node);

private:

	//! The path to the isotope database
	std::string _database;

	//! The registry that will store the created Isotope objects
	isotopeMap _registry;

	//! The map that stores the units for the properties of the database
	unitsMap _units;
};

template<typename T>
T IsotopeManager::getProperty(const std::string& name, const std::string& prop) const
{
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

	BOOST_FOREACH(property_tree::ptree::value_type const& v, root.get_child("isotopes"))
	{
		// If the current node is not an 'isotope', skip it
		if (v.first.compare("isotope")!=0)
			continue;

		// If the node attribute 'name' matches the input name, fetches its subnode that matches the input property and returns its value.
		if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
		{
			try
			{
				return v.second.get<T>(prop);
			}
			catch (const std::runtime_error& error)
			{
				throw SX::Kernel::Error<IsotopeManager>(error.what());
			}
		}
	}
	throw SX::Kernel::Error<IsotopeManager>("No match for entry "+name);
}

template<typename T>
isotopeSet IsotopeManager::getIsotopes(const std::string& prop, T value)
{
	isotopeSet isSet;

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

	// Loop over the nodes of the isotopes XML database
	BOOST_FOREACH(property_tree::ptree::value_type const& v, root.get_child("isotopes"))
	{
		// If the node is not an isotope node, ignore it
		if (v.first.compare("isotope")!=0)
			continue;

		// Search for the target property among the available ones for this isotope
		boost::optional<T> opt=v.second.get_optional<T>(prop);
		if (opt)
		{
			// Case where the property was found and matches the target value
			if (opt.get()==value)
			{
				std::string name=v.second.get<std::string>("<xmlattr>.name");
				auto it=_registry.find(name);
				if (it!=_registry.end())
					isSet.insert(it->second);
				else
				{
					sptrIsotope isotope=buildIsotope(v.second);
					_registry.insert(isotopePair(name,isotope));
					isSet.insert(isotope);
				}
			}
		}
	}

	// If the search gave no match, throws
	if (isSet.empty())
		throw SX::Kernel::Error<IsotopeManager>("No isotopes matches property "+prop+" with value "+value);

	return isSet;
}

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ISOTOPEMANAGER_H_ */
