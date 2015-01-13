#include <algorithm>
#include <functional>
#include <numeric>

#include "Error.h"
#include "Element.h"
#include "Material.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

double Material::tolerance=1.0e-6;

std::string Material::database="materials.xml";

materialMap Material::registry=materialMap();

std::map<std::string,Material::State> Material::_toState={
		{"solid",Material::State::Solid},
		{"liquid",Material::State::Liquid},
		{"gaz",Material::State::Gaz}
};

std::map<std::string,Material::FillingMode> Material::_toFillingMode={
		{"mass_fraction",Material::FillingMode::MassFraction},
		{"mole_fraction",Material::FillingMode::MoleFraction},
		{"number_of_atoms",Material::FillingMode::NumberOfAtoms}
};

Material* Material::buildFromDatabase(const std::string& name)
{
	auto it=registry.find(name);
	if (it!=registry.end())
		return it->second;
	else
	{
		ptree root;
		read_xml(database,root);

		BOOST_FOREACH(ptree::value_type const& v, root.get_child("materials"))
		{
			if (v.first.compare("material")!=0)
				continue;

			if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
				return readMaterial(v.second);
		}
	}
	throw SX::Kernel::Error<Material>("Material "+name+" is not registered in the materials database");

}

unsigned int Material::getNRegisteredMaterials()
{
	return registry.size();
}

Material* Material::readMaterial(const ptree& node)
{

	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	// Get the physical states and the filling modes of the material to be constructed
	State state=_toState.at(node.get<std::string>("<xmlattr>.state","solid"));
	FillingMode fMode=_toFillingMode.at(node.get<std::string>("<xmlattr>.filling_mode","mass_fraction"));
	Material* material=new Material(node.get<std::string>("<xmlattr>.name"),state,fMode);
	BOOST_FOREACH(ptree::value_type const& v, node)
	{
		if (v.first.compare("material")==0)
		{
			Material* submaterial;
			boost::optional<std::string> submat=v.second.get_optional<std::string>("<xmlattr>.material");
			if (submat)
				submaterial=buildFromDatabase(submat.get());
			else
				submaterial=readMaterial(v.second);

			if (fMode==FillingMode::MassFraction || fMode==FillingMode::MoleFraction)
			{
				boost::optional<const ptree&> fraction = v.second.get_child_optional("fraction");
				SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();
				if (fraction)
				{
					ptree node=fraction.get();
					double units=um->get(node.get<std::string>("<xmlattr>.units","%"));
					material->addMaterial(submaterial,node.get_value<double>()*units);
				}
				else
					throw SX::Kernel::Error<Material>("Misformatted XML 'material' node: must have 'fraction' tag");
			}
			else
			{
				boost::optional<const ptree&> nAtoms = v.second.get_child_optional("natoms");
				if (nAtoms)
				{
					ptree node=nAtoms.get();
					material->addMaterial(submaterial,node.get_value<double>());
				}
				else
					throw SX::Kernel::Error<Material>("Misformatted XML 'material' node: must have 'natoms' tag");

			}

		}
		else if (v.first.compare("element")==0)
		{
			Element* element=Element::readElement(v.second);
			if (fMode==FillingMode::MassFraction || fMode==FillingMode::MoleFraction)
			{
				boost::optional<const ptree&> fraction = v.second.get_child_optional("fraction");
				if (fraction)
				{
					ptree node=fraction.get();
					double units=um->get(node.get<std::string>("<xmlattr>.units","%"));
					material->addElement(element,node.get_value<double>()*units);
				}
				else
					throw SX::Kernel::Error<Material>("Misformatted XML 'element' node: must have 'fraction' tag");
			}
			else
			{
				boost::optional<const ptree&> nAtoms = v.second.get_child_optional("natoms");
				if (nAtoms)
				{
					ptree node=nAtoms.get();
					material->addElement(element,node.get_value<double>());
				}
				else
					throw SX::Kernel::Error<Material>("Misformatted XML 'element' node: must have 'natoms' tag");
			}
		}
	}

	if (material->_state==State::Gaz)
	{
		boost::optional<const ptree&> density = node.get_child_optional("density");
		if (density)
		{
			ptree node=density.get();
			double units=um->get(node.get<std::string>("<xmlattr>.units","kg/m3"));
			material->setDensity(node.get_value<double>()*units);
		}
		else
		{
			const ptree& pNode=node.get_child("pressure");
			double pUnits=um->get(pNode.get<std::string>("<xmlattr>.units","Pa"));
			double pressure=pNode.get_value<double>()*pUnits;

			const ptree& tNode=node.get_child("temperature");
			double tUnits=um->get(tNode.get<std::string>("<xmlattr>.units","K"));
			double temperature=tNode.get_value<double>()*tUnits;

			material->setDensity(pressure,temperature);
		}
	}
	else
	{
		const ptree& density = node.get_child("density");
		double units=um->get(density.get<std::string>("<xmlattr>.units","kg/m3"));
		material->setDensity(density.get_value<double>()*units);
	};

	registry.insert(materialPair(material->_name,material));

	return material;
}

bool Material::hasMaterial(const std::string& name)
{
	auto it=registry.find(name);
	return (it!=registry.end());
}

Material::Material(const std::string& name, State state, FillingMode fillingMode)
: _name(name),
  _density(0.0),
  _state(state),
  _fillingMode(fillingMode),
  _elements()
{
	registerMaterial(this);
}

Material::~Material()
{
}

bool Material::operator==(const Material& other) const
{

	elementContentsMap mf1=getMassFractions();
	elementContentsMap mf2=other.getMassFractions();

	return (_density==other._density) &&
			(_state==other._state) &&
			(mf1.size() == mf2.size()) &&
			std::equal(mf1.begin(),
					   mf1.end(),
					   mf2.begin(),
					   [] (elementContentsPair a, elementContentsPair b) { return a.first==b.first && std::abs(a.second-b.second)<tolerance;});
}

void Material::registerMaterial(Material* material)
{
	if (hasMaterial(material->_name))
		throw SX::Kernel::Error<Element>("The registry already contains a material with "+material->_name+" name");
	registry.insert(materialPair(material->_name,material));
}

void Material::addElement(Element* element, double fraction)
{

	if (_fillingMode==FillingMode::MassFraction || _fillingMode==FillingMode::MoleFraction)
	{
		if (fraction<=0 || fraction>1)
			throw SX::Kernel::Error<Material>("Invalid value for mole fraction");
		double sum=std::accumulate(std::begin(_elements),
				                    std::end(_elements),
				                    fraction,
				                    [](const double previous, const elementContentsPair& p) { return previous+p.second; });
		if (sum>(1.0+tolerance))
			throw SX::Kernel::Error<Material>("The sum of mole fractions exceeds 1.0");
	}

	// If the material already contains the element, return
	auto it=_elements.find(element);
	if (it!=_elements.end())
		it->second += fraction;
	else
		_elements.insert(elementContentsPair(element,fraction));

	return;
}

void Material::addElement(const std::string& name, double fraction)
{
	Element* el=Element::buildFromDatabase(name);

	addElement(el,fraction);

	return;
}

void Material::addMaterial(Material* material, double fraction)
{
	if (_fillingMode == FillingMode::NumberOfAtoms)
		throw SX::Kernel::Error<Material>("Invalid filling mode");

	for (auto it : material->getMassFractions())
		addElement(it.first,fraction*it.second);
}

void Material::addMaterial(const std::string& name, double fraction)
{
	Material* mat=Material::buildFromDatabase(name);

	addMaterial(mat,fraction);

	return;
}

elementContentsMap Material::getMassFractions() const
{

	elementContentsMap fractions;

	switch(_fillingMode)
	{
	case FillingMode::MoleFraction:
	{
		for (auto it1=_elements.begin();it1!=_elements.end();++it1)
		{
			double xi=1.0;
			double mi=it1->first->getMolarMass();
			for (auto it2=_elements.begin();it2!=_elements.end();++it2)
			{
				if (it1==it2)
					continue;
				double mj=it2->first->getMolarMass();
				xi+=(it2->second/it1->second)*(mj/mi);
				fractions.insert(elementContentsPair(it1->first,1.0/xi));
			}
		}
		break;
	}

	case FillingMode::MassFraction:
	{
		fractions = _elements;
		break;
	}

	case FillingMode::NumberOfAtoms:
	{
		double totalMass=0.0;
		for (auto it=_elements.begin();it!=_elements.end();++it)
		{
			double fimi=it->first->getMolarMass()*it->second;
			fractions.insert(elementContentsPair(it->first,fimi));
			totalMass+=fimi;
		}
		for (auto& it : fractions)
			it.second/=totalMass;
		break;
	}

	}

	return fractions;
}

elementContentsMap Material::getMoleFractions() const
{

	elementContentsMap fractions;

	switch(_fillingMode)
	{
	case FillingMode::MoleFraction:
	{
		fractions = _elements;
		break;
	}

	case FillingMode::MassFraction:
	{
		for (auto it1=_elements.begin();it1!=_elements.end();++it1)
		{
			double xi=1.0;
			double mi=it1->first->getMolarMass();
			for (auto it2=_elements.begin();it2!=_elements.end();++it2)
			{
				if (it1==it2) continue;
				double mj=it2->first->getMolarMass();
				xi+=(it2->second/it1->second)*(mi/mj);
				fractions.insert(std::pair<Element*,double>(it1->first,1.0/xi));
			}
		}
		break;
	}

	case FillingMode::NumberOfAtoms:
	{
		double nAtoms=std::accumulate(std::begin(_elements),
				                       std::end(_elements),
				                       0.0,
				                       [](double previous, const elementContentsPair& p) {return previous+p.second;});
		for (auto it=_elements.begin();it!=_elements.end();++it)
			fractions.insert(std::pair<Element*,double>(it->first,it->second/nAtoms));
		break;
	}

	}

	return fractions;
}

elementContentsMap Material::getNAtomsPerVolume() const
{
	elementContentsMap nAtoms;

	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	double fact=um->get("avogadro")*_density;

	for (auto it : getMassFractions())
		nAtoms.insert(elementContentsPair(it.first,fact*it.second/it.first->getMolarMass()));

	return nAtoms;
}

double Material::getNAtomsTotalPerVolume() const
{
	elementContentsMap nAtoms=getNAtomsPerVolume();

	return std::accumulate(std::begin(nAtoms),
			                std::end(nAtoms),
			                0.0,
			                [](double previous, const elementContentsPair& p){return previous+p.second;});
}

elementContentsMap Material::getNElectronsPerVolume() const
{
	elementContentsMap nAtoms;

	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	double fact=um->get("avogadro")*_density;

	for (auto it : getMassFractions())
		nAtoms.insert(elementContentsPair(it.first,static_cast<double>(fact*it.first->getNElectrons())*it.second/it.first->getMolarMass()));

	return nAtoms;
}

double Material::getNElectronsTotalPerVolume() const
{
	elementContentsMap nElectrons=getNElectronsPerVolume();

	return std::accumulate(std::begin(nElectrons),
			                std::end(nElectrons),
			                0.0,
			                [](double previous, const elementContentsPair& p){return previous+p.second;});
}

const std::string& Material::getName() const
{
	return _name;
}

unsigned int Material::getNElements() const
{
	return _elements.size();
}

double Material::getDensity() const
{
	return _density;
}

void Material::setDensity(double density)
{
	if (density<=0)
		throw SX::Kernel::Error<Material>("Negative density value");
	_density=density;
}

void Material::setDensity(double pressure, double temperature)
{

	if (_state!=State::Gaz)
		throw SX::Kernel::Error<Material>("Setting the density from pressure and temperature only applies for materials in gaz state");;

	if (pressure<=0 || temperature<=0)
		throw SX::Kernel::Error<Material>("Negative pressure and/or temperature values");

	SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	elementContentsMap m=getMoleFractions();

	double moleDensity=pressure/um->get("R")/temperature;
	double density=0.0;

	for (auto it : m)
		density+=moleDensity*it.second*it.first->getMolarMass();

	setDensity(density);
}

} // end namespace Chemistry

} // end namespace SX
