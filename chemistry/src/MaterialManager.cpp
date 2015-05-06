#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>

#include "ChemicalFormulaParser.h"
#include "IsotopeManager.h"
#include "Element.h"
#include "ElementManager.h"
#include "Error.h"
#include "Material.h"
#include "MaterialManager.h"
#include "MaterialsFactory.h"
#include "Path.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

MaterialManager::MaterialManager() : _registry()
{
	// The default path for the elements database is $HOME/.nsxtool/databases/elements.xml
	filesystem::path p(SX::Utils::Path::getApplicationDataPath());
	p/="databases";
	p/="materials.xml";
	_database = p.string();
}

MaterialManager::~MaterialManager()
{
	cleanRegistry();
}

void MaterialManager::cleanRegistry()
{
	for (auto it=_registry.begin();it!=_registry.end();)
	{
		if (it->second.unique())
			it=_registry.erase(it);
		else
			++it;
	}
}

sptrMaterial MaterialManager::buildEmptyMaterial(const std::string& name, BuildingMode mode)
{
	// Check first if an element with this name has already been registered
	auto it=_registry.find(name);
	if (it!=_registry.end())
	{
		if (it->second->getBuildingMode()!=mode)
			throw SX::Kernel::Error<MaterialManager>("A material that matches "+name+" name is already registered but with a different building mode.");
		return it->second;
	}

	// Otherwise built it from scratch.
	sptrMaterial mat(Material::create(name,mode));
	_registry.insert(MaterialsRegistry::value_type(name,mat));
	return mat;
}

sptrMaterial MaterialManager::buildMaterialFromChemicalFormula(std::string formula)
{

	namespace qi=boost::spirit::qi;

	SX::Chemistry::formula chemicalContents;

	ChemicalFormulaParser<std::string::iterator> parser;
	qi::phrase_parse(formula.begin(),formula.end(),parser,qi::blank,chemicalContents);

	ElementManager* emgr=ElementManager::Instance();

	sptrMaterial mat(Material::create(formula,BuildingMode::Stoichiometry));

	for (auto cc : chemicalContents)
	{
		std::string symbol=boost::fusion::at_c<0>(cc);
		std::string isotope=boost::fusion::at_c<1>(cc);
		double stoichiometry=boost::fusion::at_c<2>(cc);

		std::string eName=symbol+isotope;

		sptrElement element(emgr->getElement(eName));

		if (element->isEmpty())
			throw Kernel::Error<MaterialManager>("The element "+eName+" is neither a natural element, neither a known isotope.");

		mat->addElement(element,stoichiometry);
	}

	return mat;
}

sptrMaterial MaterialManager::buildMaterial(const property_tree::ptree& node)
{
	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	// Gets the "name" of the Material to be built
	std::string name=node.get<std::string>("<xmlattr>.name");

	// Get the building mode of the material to be constructed
	BuildingMode buildingMode=Material::stringToBuildingMode.at(node.get<std::string>("<xmlattr>.building_mode"));

	// Create an empty Material
	sptrMaterial material(Material::create(name,buildingMode));

	// Loop over the subnodes of the node
	BOOST_FOREACH(const property_tree::ptree::value_type& v, node)
	{
		// Case where the current subnode is a "material" node. A new Material will be built and added as a component of the empty Material
		if (v.first.compare("material")==0)
		{
			sptrMaterial subMaterial = buildMaterial(v.second);
			name=v.second.get<std::string>("<xmlattr>.name");

			if (buildingMode==BuildingMode::MassFraction || buildingMode==BuildingMode::MolarFraction)
			{
				const property_tree::ptree& fraction = v.second.get_child("amount");
				double units=um->get(fraction.get<std::string>("<xmlattr>.units","%"));
				material->addMaterial(subMaterial,fraction.get_value<double>()*units);
			}
			else if (buildingMode==BuildingMode::PartialPressure)
			{
				const property_tree::ptree& pressure = v.second.get_child("amount");
				double units=um->get(pressure.get<std::string>("<xmlattr>.units","Pa"));
				material->addMaterial(subMaterial,pressure.get_value<double>()*units);
			}
			else if (buildingMode==BuildingMode::Stoichiometry)
			{
				const property_tree::ptree& nAtoms = v.second.get_child("amount");
				material->addMaterial(subMaterial,nAtoms.get_value<double>());
			}
			else
				throw SX::Kernel::Error<MaterialManager>("Unknown material building mode");
		}
		else if (v.first.compare("element")==0)
		{
			ElementManager* mgr=ElementManager::Instance();
			sptrElement element;
			name=v.second.get<std::string>("<xmlattr>.name");
			// If the element is stored in the elements registry just get it
			if (mgr->hasElement(name))
				element=mgr->getElement(name);
			// Otherwise parses the XMl node, build and register an new element out of it
			else
				element=mgr->buildElement(v.second);

			if (buildingMode==BuildingMode::MassFraction || buildingMode==BuildingMode::MolarFraction)
			{
				const property_tree::ptree& fraction = v.second.get_child("amount");
				double units=um->get(fraction.get<std::string>("<xmlattr>.units","%"));
				material->addElement(element,fraction.get_value<double>()*units);
			}
			else if (buildingMode==BuildingMode::PartialPressure)
			{
				const property_tree::ptree& pressure = v.second.get_child("amount");
				double units=um->get(pressure.get<std::string>("<xmlattr>.units","Pa"));
				material->addElement(element,pressure.get_value<double>()*units);
			}
			else if (buildingMode==BuildingMode::Stoichiometry)
			{
				const property_tree::ptree& nAtoms = v.second.get_child("amount");
				material->addElement(element,nAtoms.get_value<double>());
			}
			else
				throw SX::Kernel::Error<MaterialManager>("Unkown material building mode");
		}
	}

	return material;

}

sptrMaterial MaterialManager::getMaterial(const std::string& name)
{
	// Check first if a Material with this name has already been registered
	auto it=_registry.find(name);
	if (it!=_registry.end())
		return it->second;

	// Otherwise build the Material from the materials XML database

	property_tree::ptree root;
	try
	{
		xml_parser::read_xml(_database,root);
	}
	catch (const std::runtime_error& error)
	{
		throw SX::Kernel::Error<MaterialManager>(error.what());
	}

	BOOST_FOREACH(const property_tree::ptree::value_type& v, root.get_child("materials"))
	{
		if (v.first.compare("material")!=0)
			continue;

		// Once the XML node has been found, build a Material from it and register it
		if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
		{
			sptrMaterial material=buildMaterial(v.second);
			_registry.insert(MaterialsRegistry::value_type(name,material));
			return material;
		}
	}
	throw SX::Kernel::Error<MaterialManager>("No material with "+name+" could be found in the database.");
}

void MaterialManager::setDatabasePath(const std::string& path)
{

	// The new path is the same than the old path, do nothing
	if (path.compare(_database)==0)
		return;

	// The given path does not exists, throws
	if (!filesystem::exists(path))
		throw SX::Kernel::Error<MaterialManager>("Invalid path for material database.");

	_database=path;
}

unsigned int MaterialManager::getNMaterials() const
{
	return _registry.size();
}

bool MaterialManager::hasMaterial(const std::string& name) const
{
	auto it=_registry.find(name);
	return (it!=_registry.end());
}

void MaterialManager::saveRegistry(std::string filename) const
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
		root.add("materials","");
	}

	property_tree::ptree& materialsNode=root.get_child("materials");

	for (const auto& m : _registry)
	{
		auto mnode = m.second->writeToXML();
		materialsNode.add_child("material",mnode);
	}

	if (filename.empty())
		filename=_database;

	boost::property_tree::xml_writer_settings<char> settings('\t', 1);
	xml_parser::write_xml(filename,root);
}

} // end namespace Chemistry

} // end namespace SX
