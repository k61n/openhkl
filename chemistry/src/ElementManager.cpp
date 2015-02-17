#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Error.h"
#include "Element.h"
#include "ElementManager.h"
#include "Path.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

// Namespaces
namespace filesystem=boost::filesystem;
namespace xml_parser=boost::property_tree::xml_parser;

ElementManager::ElementManager() : _registry()
{
	// The default path for the elements database is $HOME/.nsxtool/databases/elements.xml
	filesystem::path p(SX::Utils::Path::getApplicationDataPath());
	p/="databases";
	p/="elements.xml";
	_database = p.string();
}

ElementManager::~ElementManager()
{
	cleanRegistry();
}

void ElementManager::cleanRegistry()
{
	for (auto& p : _registry)
		delete p.second;

	_registry.clear();
}

void ElementManager::setDatabasePath(const std::string& path)
{

	// The new path is the same than the old path, do nothing
	if (path.compare(_database)==0)
		return;

	// The given path does not exists, throws
	if (!filesystem::exists(path))
		throw SX::Kernel::Error<ElementManager>("Invalid path for elements database.");

	_database=path;
}

Element* ElementManager::buildElement(const std::string& name, const std::string& symbol)
{
	// Check first if an element with this name has already been registered
	auto it=_registry.find(name);
	if (it!=_registry.end())
		throw SX::Kernel::Error<ElementManager>("An element with name "+name+" is already registered in the database.");

	// Otherwise built it from scratch.
	Element* element=Element::create(name,symbol);
	_registry.insert(elementPair(name,element));
	return element;
}

Element* ElementManager::buildElement(const property_tree::ptree& node)
{
	Element* element;

	std::string name=node.get<std::string>("<xmlattr>.name");

	// If the element node has an attribute symbol then the element is built from its natural isotopes
	boost::optional<const property_tree::ptree&> symbol = node.get_child_optional("symbol");
	if (symbol)
		element=buildElement(name,symbol.get().get_value<std::string>());
	else
	{
		SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

		// Build an empty Element
		element=buildElement(name);
		// Loop over the 'isotope' nodes
		BOOST_FOREACH(const property_tree::ptree::value_type& subnode, node)
		{
			if (subnode.first.compare("isotope")!=0)
				continue;

			std::string isoName=subnode.second.get<std::string>("<xmlattr>.name");
			// If the isotope node has an 'abundance' node then use its value to set the abundance of the corresponding isotope in the element being built
			boost::optional<const property_tree::ptree&> abundance = subnode.second.get_child_optional("abundance");
			if (abundance)
			{
				property_tree::ptree abundanceNode=abundance.get();
				double units=um->get(abundanceNode.get<std::string>("<xmlattr>.units","%"));
				element->addIsotope(isoName,abundanceNode.get_value<double>()*units);
			}
			// Otherwise use the natural abundance found in the isotopes XML database
			else
				element->addIsotope(isoName);
		}
	}
	_registry.insert(elementPair(name,element));
	return element;

}

Element* ElementManager::findElement(const std::string& name)
{

	// Looks first for the element in the registry
	auto it=_registry.find(name);
	if (it!=_registry.end())
		return it->second;

	// Looks for the element in the elements XML database

	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<ElementManager>(error.what());
	}

	BOOST_FOREACH(const property_tree::ptree::value_type& node, root.get_child("elements"))
	{
		if (node.first.compare("element")!=0)
			continue;

		if (node.second.get<std::string>("<xmlattr>.name").compare(name)==0)
		{
			Element* el=buildElement(node.second);
			return el;
		}
	}

	throw SX::Kernel::Error<ElementManager>("Element "+name+" could not be found neither in the registry neither in the elements XML database.");

}

bool ElementManager::hasElement(const std::string& name) const
{
	auto it=_registry.find(name);
	return (it!=_registry.end());
}

unsigned int ElementManager::getNRegisteredElements() const
{
	return _registry.size();
}

} // end namespace Chemistry

} // end namespace SX
