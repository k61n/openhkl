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
#include "IMaterial.h"
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

sptrMaterial MaterialManager::buildEmptyMaterial(const std::string& name, IMaterial::State state, IMaterial::BuildingMode buildingMode)
{
	// Check first if an element with this name has already been registered
	auto it=_registry.find(name);
	if (it!=_registry.end())
	{
		if (it->second->getBuildingMode()!=buildingMode || it->second->getState()!= state)
			throw SX::Kernel::Error<MaterialManager>("A material that matches "+name+" name is already registered but with a different chemical state and/or building mode.");
		return it->second;
	}

	MaterialsFactory* matFactory=MaterialsFactory::Instance();

	// Otherwise built it from scratch.
	sptrMaterial mat(matFactory->create(buildingMode,name,state));
	_registry.insert(strToMaterialPair(name,mat));
	return mat;
}

sptrMaterial MaterialManager::buildMaterialFromChemicalFormula(std::string formula, IMaterial::State state)
{

	namespace qi=boost::spirit::qi;

	SX::Utils::formula chemicalContents;

	SX::Utils::ChemicalFormulaParser<std::string::iterator> parser;
	qi::phrase_parse(formula.begin(),formula.end(),parser,qi::blank,chemicalContents);

	ElementManager* emgr=ElementManager::Instance();

	MaterialsFactory* matFactory=MaterialsFactory::Instance();

	sptrMaterial mat(matFactory->create(IMaterial::BuildingMode::Stoichiometry,formula,state));

	for (auto cc : chemicalContents)
	{
		std::string symbol=boost::fusion::at_c<0>(cc);
		std::string isotope=boost::fusion::at_c<1>(cc);
		unsigned int nAtoms=boost::fusion::at_c<2>(cc);

		sptrElement element;

		// Case of an element that has to be built from its natural isotopes
		if (isotope.empty())
			element = emgr->getElement(symbol,symbol);
		else
		{
			element = emgr->getElement(symbol+isotope);
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

	// Get the chemical state and the building mode of the material to be constructed
	IMaterial::State chemicalState=IMaterial::strToState.at(node.get<std::string>("<xmlattr>.chemical_state"));
	IMaterial::BuildingMode buildingMode=IMaterial::strToBuildingMode.at(node.get<std::string>("<xmlattr>.building_mode"));

	MaterialsFactory* matFactory=MaterialsFactory::Instance();

	// Create an empty Material
	sptrMaterial material(matFactory->create(buildingMode,name,chemicalState));

	// Loop over the subnodes of the node
	BOOST_FOREACH(const property_tree::ptree::value_type& v, node)
	{
		// Case where the current subnode is a "material" node. A new Material will be built and added as a component of the empty Material
		if (v.first.compare("material")==0)
		{
			sptrMaterial component;

			boost::optional<std::string> submat=v.second.get_optional<std::string>("<xmlattr>.database");
			if (submat)
				component=getMaterial(submat.get());
			else
			{
				name=v.second.get<std::string>("<xmlattr>.name");
				component=getMaterial(name);
			}

			if (buildingMode==IMaterial::BuildingMode::MassFractions || buildingMode==IMaterial::BuildingMode::MolarFractions)
			{
				const property_tree::ptree& fraction = v.second.get_child("contents");
				double units=um->get(fraction.get<std::string>("<xmlattr>.units","%"));
				material->addMaterial(component,fraction.get_value<double>()*units);
			}
			else if (buildingMode==IMaterial::BuildingMode::PartialPressures)
			{
				const property_tree::ptree& pressure = v.second.get_child("contents");
				double units=um->get(pressure.get<std::string>("<xmlattr>.units","Pa"));
				material->addMaterial(component,pressure.get_value<double>()*units);
			}
			else if (buildingMode==IMaterial::BuildingMode::Stoichiometry)
			{
				const property_tree::ptree& nAtoms = v.second.get_child("contents");
				material->addMaterial(component,nAtoms.get_value<double>());
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

			if (buildingMode==IMaterial::BuildingMode::MassFractions || buildingMode==IMaterial::BuildingMode::MolarFractions)
			{
				const property_tree::ptree& fraction = v.second.get_child("contents");
				double units=um->get(fraction.get<std::string>("<xmlattr>.units","%"));
				material->addElement(element,fraction.get_value<double>()*units);
			}
			else if (buildingMode==IMaterial::BuildingMode::PartialPressures)
			{
				const property_tree::ptree& pressure = v.second.get_child("contents");
				double units=um->get(pressure.get<std::string>("<xmlattr>.units","Pa"));
				material->addElement(element,pressure.get_value<double>()*units);
			}
			else if (buildingMode==IMaterial::BuildingMode::Stoichiometry)
			{
				const property_tree::ptree& nAtoms = v.second.get_child("contents");
				material->addElement(element,nAtoms.get_value<double>());
			}
			else
				throw SX::Kernel::Error<MaterialManager>("Unkown material building mode");
		}
	}

	if (material->getBuildingMode()!=IMaterial::BuildingMode::PartialPressures)
	{
		const property_tree::ptree& density = node.get_child("mass_density");
		double units=um->get(density.get<std::string>("<xmlattr>.units","kg/m3"));
		material->setMassDensity(density.get_value<double>()*units);
	};

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
			_registry.insert(strToMaterialPair(name,material));
			return material;
		}
	}

	MaterialsFactory* matFactory=MaterialsFactory::Instance();

	sptrMaterial material(matFactory->create(IMaterial::BuildingMode::MassFractions,name,IMaterial::State::Solid));
	_registry.insert(strToMaterialPair(name,material));
	return material;
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

	property_tree::ptree& elementsNode=root.get_child("materials");

	for (const auto& e : _registry)
		e.second->writeToXML(elementsNode);

	if (filename.empty())
		filename=_database;

	boost::property_tree::xml_writer_settings<char> settings('\t', 1);
	xml_parser::write_xml(filename,root);
}

} // end namespace Chemistry

} // end namespace SX
