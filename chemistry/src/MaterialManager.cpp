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

sptrMaterial MaterialManager::buildMaterial(const std::string& name, Material::State state, Material::FillingMode fillingMode)
{
	// Check first if an element with this name has already been registered
	auto it=_registry.find(name);
	if (it!=_registry.end())
		return it->second;

	// Otherwise built it from scratch.
	sptrMaterial mat=Material::create(name,state,fillingMode);
	_registry.insert(materialPair(name,mat));
	return mat;
}

sptrMaterial MaterialManager::buildMaterialFromChemicalFormula(std::string formula, Material::State state)
{

	namespace qi=boost::spirit::qi;

	SX::Utils::formula chemicalContents;

	SX::Utils::ChemicalFormulaParser<std::string::iterator> parser;
	qi::phrase_parse(formula.begin(),formula.end(),parser,qi::blank,chemicalContents);

	ElementManager* emgr=ElementManager::Instance();

	sptrMaterial mat=Material::create(formula,state,Material::FillingMode::NumberOfAtoms);

	for (auto cc : chemicalContents)
	{
		std::string symbol=boost::fusion::at_c<0>(cc);
		std::string isotope=boost::fusion::at_c<1>(cc);
		unsigned int nAtoms=boost::fusion::at_c<2>(cc);

		sptrElement element;

		if (isotope.empty())
		{
			element = emgr->buildElement(symbol,symbol);
			if (element->isEmpty())
				throw SX::Kernel::Error<MaterialManager>("The element "+symbol+"is not a valid element name.");
		}
		else
		{
			element = emgr->buildElement(symbol,symbol+isotope);
			if (element->isEmpty())
			{
				try
				{
					element->addIsotope(symbol+isotope);
				}
				catch(const SX::Kernel::Error<IsotopeManager>& error)
				{
					throw SX::Kernel::Error<MaterialManager>(error.what());
				}
			}
		}

		mat->addElement(element,nAtoms);
	}

	return mat;
}

sptrMaterial MaterialManager::buildMaterial(const property_tree::ptree& node)
{
	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	// Gets the "name" of the Material to be built
	std::string name=node.get<std::string>("<xmlattr>.name");

	// Get the physical state and the filling modes of the material to be constructed
	Material::State state=Material::s_toState.at(node.get<std::string>("<xmlattr>.state","solid"));
	Material::FillingMode fMode=Material::s_toFillingMode.at(node.get<std::string>("<xmlattr>.filling_mode","mass_fraction"));

	// Create an empty Material
	sptrMaterial material=Material::create(name,state,fMode);

	// Loop over the subnodes of the node
	BOOST_FOREACH(const property_tree::ptree::value_type& v, node)
	{
		// Case where the current subnode is a "material" node. A new Material will be built and added as a component of the empty Material
		if (v.first.compare("material")==0)
		{
			sptrMaterial component;

			boost::optional<std::string> submat=v.second.get_optional<std::string>("<xmlattr>.database");
			if (submat)
				component=findMaterial(submat.get());
			else
			{
				name=v.second.get<std::string>("<xmlattr>.name");
				try
				{
					component=findMaterial(name);
				}
				catch(const SX::Kernel::Error<MaterialManager>& e)
				{
					component=buildMaterial(v.second);
					_registry.insert(materialPair(name,component));
				}
			}

			if (fMode==Material::FillingMode::MassFraction || fMode==Material::FillingMode::MoleFraction)
			{
				const property_tree::ptree& fraction = v.second.get_child("contents");
				double units=um->get(fraction.get<std::string>("<xmlattr>.units","%"));
				material->addMaterial(component,fraction.get_value<double>()*units);
			}
			else if (fMode==Material::FillingMode::PartialPressure)
			{
				const property_tree::ptree& pressure = v.second.get_child("contents");
				double units=um->get(pressure.get<std::string>("<xmlattr>.units","Pa"));
				material->addMaterial(component,pressure.get_value<double>()*units);
			}
			else if (fMode==Material::FillingMode::NumberOfAtoms)
			{
				const property_tree::ptree& nAtoms = v.second.get_child("contents");
				material->addMaterial(component,nAtoms.get_value<double>());
			}
			else
				throw SX::Kernel::Error<MaterialManager>("Unknown filling mode");
		}
		else if (v.first.compare("element")==0)
		{
			ElementManager* mgr=ElementManager::Instance();
			sptrElement element;
			name=v.second.get<std::string>("<xmlattr>.name");
			try
			{
				element=mgr->findElement(name);
			}
			catch(const SX::Kernel::Error<ElementManager>& e)
			{
				element=mgr->buildElement(v.second);
			}
			if (fMode==Material::FillingMode::MassFraction || fMode==Material::FillingMode::MoleFraction)
			{
				const property_tree::ptree& fraction = v.second.get_child("contents");
				double units=um->get(fraction.get<std::string>("<xmlattr>.units","%"));
				material->addElement(element,fraction.get_value<double>()*units);
			}
			else if (fMode==Material::FillingMode::PartialPressure)
			{
				const property_tree::ptree& pressure = v.second.get_child("contents");
				double units=um->get(pressure.get<std::string>("<xmlattr>.units","Pa"));
				material->addElement(element,pressure.get_value<double>()*units);
			}
			else if (fMode==Material::FillingMode::NumberOfAtoms)
			{
				const property_tree::ptree& nAtoms = v.second.get_child("contents");
				material->addElement(element,nAtoms.get_value<double>());
			}
			else
				throw SX::Kernel::Error<MaterialManager>("Unkown filling mode");
		}
	}

	if (material->getFillingMode()!=Material::FillingMode::PartialPressure)
	{
		const property_tree::ptree& density = node.get_child("density");
		double units=um->get(density.get<std::string>("<xmlattr>.units","kg/m3"));
		material->setDensity(density.get_value<double>()*units);
	};

	_registry.insert(materialPair(name,material));

	return material;

}

sptrMaterial MaterialManager::findMaterial(const std::string& name)
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
			return buildMaterial(v.second);
	}
	throw SX::Kernel::Error<MaterialManager>("Material "+name+" is not registered in the materials database");
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

unsigned int MaterialManager::getNRegisteredMaterials() const
{
	return _registry.size();
}

bool MaterialManager::hasMaterial(const std::string& name) const
{
	auto it=_registry.find(name);
	return (it!=_registry.end());
}

std::set<std::string> MaterialManager::getDatabaseNames() const
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

	BOOST_FOREACH(const property_tree::ptree::value_type& node, root.get_child("materials"))
	{
		if (node.first.compare("material")!=0)
			continue;
		names.insert(node.second.get<std::string>("<xmlattr>.name"));
	}

	return names;
}

void MaterialManager::synchronizeDatabase(std::string filename) const
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

	std::set<std::string> dbNames=getDatabaseNames();

	property_tree::ptree& materialsNode=root.get_child("materials");

	for (const auto& m : _registry)
	{
		auto it=dbNames.find(m.second->getName());
		if (it!=dbNames.end())
			continue;

		m.second->writeToXML(materialsNode);
	}

	if (filename.empty())
		filename=_database;

	boost::property_tree::xml_writer_settings<char> settings('\t', 1);
	xml_parser::write_xml(filename,root);

}

} // end namespace Chemistry

} // end namespace SX
