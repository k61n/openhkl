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
#include <set>
#include <string>
#include <utility>

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
typedef std::set<Isotope*> isotopeSet;
typedef std::map<std::string,Isotope*> isotopeMap;
typedef std::pair<std::string,Isotope*> isotopePair;

class IsotopeManager : public SX::Kernel::Singleton<IsotopeManager,SX::Kernel::Constructor,SX::Kernel::Destructor>
{

public:

	//! Default constructor
	IsotopeManager();

	//! Destructor
	~IsotopeManager();

	//! Returns the set of Isotopes whose property |prop| matches the value |value|
	template<typename T>
	isotopeSet getIsotopes(const std::string& prop, T value);

	//! Returns the number of registered isotopes
	unsigned int getNRegisteredIsotopes() const;

	//! Returns the value of a given property of a given isotope
	template<typename T>
	T getProperty(const std::string& name, const std::string& prop) const;

	//! Sets the path for the isotopes XML database
	void setDatabasePath(const std::string& path);

	//! Finds an isotope with a given name. If the isotope name does not match any registered isotope build it from the isotopes XML database.
	Isotope* findIsotope(const std::string& name);

	//! Returns true if an isotope with a given name is registered
	bool hasIsotope(const std::string& name) const;

private:

	//! Clean up the registry
	void cleanRegistry();

private:

	//! The path to the isotope database
	std::string _database;

	//! The registry that will store the created Isotope objects
	isotopeMap _registry;
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
					Isotope* is=Isotope::create(v.second);
					_registry.insert(isotopePair(name,is));
					isSet.insert(is);
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
