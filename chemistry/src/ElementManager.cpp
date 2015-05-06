#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Error.h"
#include "Element.h"
#include "ElementManager.h"
#include "Path.h"
#include "Units.h"
#include "IsotopeManager.h"

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

	setDatabasePath(p.string());
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

void ElementManager::setDatabasePath(std::string path)
{
	// The new path is the same than the old path, do nothing
	if (path.compare(_database)==0)
		return;

	if (path.empty())
		path=_database;

	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(path,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<ElementManager>(error.what());
	}

	_registry.clear();

	_database=path;

	BOOST_FOREACH(const property_tree::ptree::value_type& node, root.get_child("elements"))
	{
		if (node.first.compare("element")!=0)
			continue;

		buildElement(node.second);
	}
}

void ElementManager::reload()
{
	setDatabasePath();
}

sptrElement ElementManager::buildElement(const property_tree::ptree& node)
{
	sptrElement element;

	std::string name;

	// Try to get the XML attribute "name" of the current element node. If not possible, skip.
	try
	{
		name=node.get<std::string>("<xmlattr>.name");
	}
	catch(const std::runtime_error& e)
	{
		return element;
	}

	// If an element with this name is already registered, skip
	auto it=_registry.find(name);
	if (it!=_registry.end())
		return element;


	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	// Build an empty Element
	element=sptrElement(Element::create(name));
	// Loop over the 'isotope' nodes
	BOOST_FOREACH(const property_tree::ptree::value_type& subnode, node)
	{
		if (subnode.first.compare("isotope")!=0)
			continue;

		std::string isotopeName;
		try
		{
			isotopeName=subnode.second.get<std::string>("<xmlattr>.name");
		}
		catch(const std::runtime_error& e)
		{
			element.reset();
			return element;
		}

		// If the isotope node has an 'abundance' node then use its value to set the abundance of the corresponding isotope in the element being built
		boost::optional<const property_tree::ptree&> abundanceNode = subnode.second.get_child_optional("abundance");
		if (abundanceNode)
		{
			double units;
			double abundance;

			// Try to get the abundance and its units for this isotope, otherwise returns a null shared pointer
			try
			{
				property_tree::ptree n=abundanceNode.get();
				abundance=n.get_value<double>();
				units=um->get(n.get<std::string>("<xmlattr>.units","%"));
			}
			catch(const std::runtime_error& e)
			{
				element.reset();
				return element;
			}

			// Try to add the isotope, otherwise returns a null shared pointer
			try
			{
				element->addIsotope(isotopeName,abundance*units);
			}
			catch(const SX::Kernel::Error<Element>& e)
			{
				element.reset();
				return element;
			}
		}
		// Otherwise use the natural abundance found in the isotopes XML database
		else
		{
			// Try to add the isotope, otherwise returns a null shared pointer
			try
			{
				element->addIsotope(isotopeName);
			}
			catch(const SX::Kernel::Error<Element>& e)
			{
				element.reset();
				return element;
			}
		}
	}

	// Everything is OK, register the element
	_registry.insert(strToElementPair(name,element));

	return element;

}

sptrElement ElementManager::getElement(const std::string& name)
{
	// Case where an Element with this name is found in the registry
	auto it=_registry.find(name);
	if (it!=_registry.end())
		return it->second;

	sptrElement element(Element::create(name));

	_registry.insert(strToElementPair(name,element));

	return element;
}

bool ElementManager::hasElement(const std::string& name) const
{
	auto it=_registry.find(name);
	return (it!=_registry.end());
}

unsigned int ElementManager::getNElements() const
{
	return _registry.size();
}

const strToElementMap& ElementManager::getRegistry() const
{
	return _registry;
}

void ElementManager::saveRegistry(std::string filename) const
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

	property_tree::ptree& elementsNode=root.get_child("elements");

	for (const auto& e : _registry)
	{
		auto enode = e.second->writeToXML();
		root.add_child("element",enode);
	}

	if (filename.empty())
		filename=_database;

	boost::property_tree::xml_writer_settings<char> settings('\t', 1);
	xml_parser::write_xml(filename,root);
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
