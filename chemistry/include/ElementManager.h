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
#include <memory>
#include <set>
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
class MaterialManager;

// Typedefs
typedef std::shared_ptr<Element> sptrElement;
typedef std::map<std::string,sptrElement> elementMap;
typedef std::pair<std::string,sptrElement> elementPair;

// Namespaces
namespace property_tree=boost::property_tree;

class ElementManager : public SX::Kernel::Singleton<ElementManager,SX::Kernel::Constructor,SX::Kernel::Destructor>
{

private:

	friend class MaterialManager;

public:

	//! Default constructor
	ElementManager();

	//! Destructor
	~ElementManager();

	//! Sets the path for the elements XML database
	void setDatabasePath(const std::string& path);

	//! Builds and registers an element. If symbol is provided the Element will be built from its natural isotopes otherwise the Element is empty.
	//! A shared pointer to the newly created Element is returned.
	sptrElement buildNaturalElement(const std::string& name, const std::string& symbol);
	//! Returns a shared pointer to an Element with a given name. The Element is searched first in the registry then in the XML database. If not found return an empty element.
	sptrElement getElement(const std::string& name);
	//! Returns the number of registered Element objects
	unsigned int getNElementsInRegistry() const;
	//! Returns true if an Element with a given name is registered
	bool isRegistered(const std::string& name) const;
	//! Clean up the Element registry
	void cleanRegistry();

	//! Returns the name of the elements stored in the elements XML database
	std::set<std::string> getDatabaseNames() const;
	//! Returns the number of elements stored in the XMl database
	unsigned int getNElementsInDatabase() const;
	//! Save the registry
	void updateDatabase(std::string filename="") const;

private:

	//! Builds and registers an Element from an XML node
	//! A shared pointer to the newly created Element is returned.
	sptrElement buildElement(const property_tree::ptree& node);

private:

	//! The path to the element database
	std::string _database;

	//! The registry that will store the created Element objects
	elementMap _registry;

};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ELEMENTMANAGER_H_ */
