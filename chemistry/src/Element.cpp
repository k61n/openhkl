#include <algorithm>
#include <cmath>

#include "Isotope.h"
#include "Element.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

double Element::tolerance=1.0e-6;

std::string Element::database="elements.xml";

elementMap Element::registry=elementMap();

Element* Element::buildFromDatabase(const std::string& name)
{
	auto it=registry.find(name);
	if (it!=registry.end())
		return it->second;
	else
	{
		ptree root;
		read_xml(database,root);

		BOOST_FOREACH(ptree::value_type const& v, root.get_child("elements"))
		{
			if (v.first.compare("element")!=0)
				continue;

			if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
				return readElement(v.second);;
		}
	}
	throw SX::Kernel::Error<Element>("Element "+name+" is not registered in the elements database");
}

Element* Element::readElement(const ptree& node)
{

	std::string name=node.get<std::string>("<xmlattr>.name");
	auto it=registry.find(name);
	if (it!=registry.end())
		return it->second;

	Element* element;

	// if the element node has an attribute symbol then the element is built from its natural isotopes
	boost::optional<const ptree&> symbol = node.get_child_optional("symbol");
	if (symbol)
		element=new Element(name,symbol.get().get_value<std::string>());
	else
	{
		SX::Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

		element=new Element(name);
		BOOST_FOREACH(ptree::value_type const& v, node)
		{
			if (v.first.compare("isotope")!=0)
				continue;

			std::string iname=v.second.get<std::string>("<xmlattr>.name");
			boost::optional<const ptree&> abundance = v.second.get_child_optional("abundance");
			if (abundance)
			{
				ptree node=abundance.get();
				double units=um->get(node.get<std::string>("<xmlattr>.units","%"));
				element->addIsotope(iname,node.get_value<double>()*units);
			}
			else
				element->addIsotope(iname);
		}
	}

	registry.insert(elementPair(name,element));

	return element;
}

bool Element::hasElement(const std::string& name)
{
	auto it=registry.find(name);
	return (it!=registry.end());
}

unsigned int Element::getNRegisteredElements()
{
	return registry.size();
}

Element::Element() : _name(""), _isotopes(), _abundances()
{
}

Element::Element(const std::string& name, const std::string& symbol) : _name(name), _isotopes(), _abundances()
{

	if (hasElement(name))
		throw SX::Kernel::Error<Element>("The registry already contains an element with "+name+" name");

	isotopeSet isotopes=Isotope::getIsotopes<std::string>("symbol",symbol);

	_isotopes.reserve(isotopes.size());
	_abundances.reserve(isotopes.size());

	for (auto is : isotopes)
	{
		_isotopes.push_back(is);
		_abundances.push_back(is->getAbundance());
	}

	registry.insert(elementPair(name,this));
}

Element::Element(const std::string& name) : _name(name), _isotopes(), _abundances()
{
	if (hasElement(name))
		throw SX::Kernel::Error<Element>("The registry already contains an element with "+name+" name");
}

Element::~Element()
{
}

void Element::addIsotope(Isotope* isotope, double abundance)
{

	if (abundance<0.0 || abundance>1.0)
		throw SX::Kernel::Error<Element>("Invalid value for abundance");

	double sum=std::accumulate(_abundances.begin(),_abundances.end(),0.0);
	sum += abundance;
	if (sum>(1.0+tolerance))
		throw SX::Kernel::Error<Element>("The sum of abundances exceeds 1.0");

	if (!_isotopes.empty())
		// Check that the isotope is chemically compatible with the element (same number of protons)
		if (isotope->getNProtons() != _isotopes[0]->getNProtons())
			throw SX::Kernel::Error<Element>("Invalid number of protons.");

		// If the element already contains the isotope, return false
		auto it=std::find(_isotopes.begin(),_isotopes.end(),isotope);
		if (it!=_isotopes.end())
			return;

	_isotopes.push_back(isotope);

	_abundances.push_back(abundance);

	return;

}

void Element::addIsotope(const std::string& name)
{
	Isotope* is=Isotope::buildFromDatabase(name);

	addIsotope(is,is->getAbundance());

	return;
}

void Element::addIsotope(const std::string& name, double abundance)
{
	Isotope* is=Isotope::buildFromDatabase(name);

	addIsotope(is,abundance);

	return;
}

const std::string& Element::getName() const
{
	return _name;
}

unsigned int Element::getNIsotopes() const
{
	return _isotopes.size();
}

double Element::getMolarMass() const
{

	double sum=std::accumulate(_abundances.begin(),_abundances.end(),0.0);
	if (std::abs(sum-1.0)>tolerance)
		throw SX::Kernel::Error<Element>("The sum of abundances is not equal to 1.0");

	double mm(0.0);
	auto pit=_abundances.begin();
	for (auto iit=_isotopes.begin();iit!=_isotopes.end();++iit,++pit)
		mm += (*pit) * ((*iit)->getMolarMass());
	return mm/sum;
}

} // end namespace Chemistry

} // end namespace SX
