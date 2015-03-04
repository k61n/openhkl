#include <iostream>

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
	for (auto it=_registry.begin();it!=_registry.end();)
	{
		if (it->second.unique())
			it=_registry.erase(it);
		else
			++it;
	}
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

sptrElement ElementManager::buildNaturalElement(const std::string& name, const std::string& symbol)
{
	if (symbol.empty())
		throw SX::Kernel::Error<ElementManager>("A chemical symbol is necessary for building a natural element.");

	// Check first if an element with this name has already been registered
	auto it=_registry.find(name);
	if (it!=_registry.end())
	{
		if (symbol.compare(it->second->getSymbol())!=0)
			throw SX::Kernel::Error<ElementManager>("An element that matches "+name+" name is already registered but with a different chemical symbol.");
		return it->second;
	}

	// Otherwise built it from scratch.
	sptrElement element(Element::create(name,symbol));
	_registry.insert(elementPair(name,element));
	return element;
}

sptrElement ElementManager::buildElement(const property_tree::ptree& node)
{
	sptrElement element;

	std::string name=node.get<std::string>("<xmlattr>.name");

	// If the element node has an attribute symbol then the element is built from its natural isotopes
	boost::optional<const property_tree::ptree&> symbol = node.get_child_optional("symbol");
	if (symbol)
		element=buildNaturalElement(name,symbol.get().get_value<std::string>());
	else
	{
		SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

		// Build an empty Element
		element=sptrElement(Element::create(name));
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
	return element;

}

sptrElement ElementManager::getElement(const std::string& name)
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
			sptrElement element=buildElement(node.second);
			_registry.insert(elementPair(name,element));
			return element;
		}
	}

	sptrElement element(Element::create(name));
	_registry.insert(elementPair(name,element));

	return element;

}

bool ElementManager::isInRegistry(const std::string& name) const
{
	auto it=_registry.find(name);
	return (it!=_registry.end());
}

unsigned int ElementManager::getNElementsInRegistry() const
{
	return _registry.size();
}

std::set<std::string> ElementManager::getDatabaseNames() const
{

	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<ElementManager>(error.what());
	}

	std::set<std::string> names;

	BOOST_FOREACH(const property_tree::ptree::value_type& node, root.get_child("elements"))
	{
		if (node.first.compare("element")!=0)
			continue;
		names.insert(node.second.get<std::string>("<xmlattr>.name"));
	}

	return names;
}

void ElementManager::updateDatabase(std::string filename) const
{

	//! If there is no entries in the registry, nothing to save, returns
	if (_registry.empty())
		return;

	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root, boost::property_tree::xml_parser::trim_whitespace);
	}
	catch (const std::runtime_error& error)
	{
		// If the database could not be opened for whatever reasons, starts with an empty property tree
		root.add("elements","");
	}

	std::set<std::string> dbNames=getDatabaseNames();

	property_tree::ptree& elementsNode=root.get_child("elements");

	for (const auto& e : _registry)
	{
		auto it=dbNames.find(e.second->getName());
		if (it!=dbNames.end())
			continue;

		e.second->writeToXML(elementsNode);
	}

	if (filename.empty())
		filename=_database;

	boost::property_tree::xml_writer_settings<char> settings('\t', 1);
	xml_parser::write_xml(filename,root);

}

unsigned int ElementManager::getNElementsInDatabase() const
{
	unsigned int nElements(0);

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
		nElements++;
	}

	return nElements;
}

bool ElementManager::isInDatabase(const std::string& name) const
{
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
			return true;
	}

	return false;
}

void ElementManager::removeElement(const std::string& name)
{
	auto it=_registry.find(name);

	if (it!=_registry.end())
		_registry.erase(it);

	return;


}

} // end namespace Chemistry

} // end namespace SX
