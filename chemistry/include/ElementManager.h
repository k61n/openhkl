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

#ifndef NSXTOOL_ELEMENTMANAGER_H_
#define NSXTOOL_ELEMENTMANAGER_H_

#include <map>
#include <string>
#include <utility>

#include <boost/property_tree/ptree.hpp>

#include "Singleton.h"

namespace SX
{

namespace Chemistry
{

// Forward declarations
class Element;
class Isotope;

// Typedefs
typedef std::map<std::string,Element*> elementMap;
typedef std::pair<std::string,Element*> elementPair;

// Namespaces
namespace property_tree=boost::property_tree;

class ElementManager : public SX::Kernel::Singleton<ElementManager,SX::Kernel::Constructor,SX::Kernel::Destructor>
{

public:

	//! Default constructor
	ElementManager();

	//! Destructor
	~ElementManager();

	//! Sets the path for the elements XML database
	void setDatabasePath(const std::string& path);

	//! Find an Element in the registry and in the elements XML database if it is not found in the registry. If it is not found in both the registry and the database, throws.
	Element* findElement(const std::string& name);

	//! Builds and register an element. If symbol is provided the Element will be built from its natural isotopes otherwise the Element is empty.
	Element* buildElement(const std::string& name, const std::string& symbol="");

	//! Builds an Element from an XML node
	Element* buildElement(const property_tree::ptree& node);

	//! Returns true if an element with a given name is registered
	bool hasElement(const std::string& name) const;

	//! Returns the number of registered elements
	unsigned int getNRegisteredElements() const;

private:

	//! Clean up the registry
	void cleanRegistry();

private:

	//! The path to the element database
	std::string _database;

	//! The registry that will store the created Element objects
	elementMap _registry;

};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ELEMENTMANAGER_H_ */
