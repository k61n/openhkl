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

Element::Element(const std::string& name, const std::string& symbol) : _name(name), _isotopes()
{
	isotopeSet isotopes=Isotope::getIsotopes<std::string>("symbol",symbol);

	for (auto is : isotopes)
		_isotopes.insert(isotopeContentsPair(is,is->getAbundance()));

	registerElement(this);
}

Element::Element(const std::string& name) : _name(name), _isotopes()
{
	registerElement(this);
}

Element::~Element()
{
}

void Element::registerElement(Element* element)
{
	if (hasElement(element->_name))
		throw SX::Kernel::Error<Element>("The registry already contains an element with "+element->_name+" name");
	registry.insert(elementPair(element->_name,element));
}

void Element::addIsotope(Isotope* isotope, double abundance)
{
	// If the element already contains the isotope, return
	auto it=_isotopes.find(isotope);
	if (it!=_isotopes.end())
		return;

	if (abundance<0.0 || abundance>1.0)
		throw SX::Kernel::Error<Element>("Invalid value for abundance");

	double sum=std::accumulate(std::begin(_isotopes),
			                    std::end(_isotopes),
			                    0.0,
			                    [](double previous, const isotopeContentsPair& p){return previous+p.second;});
	sum += abundance;
	if (sum>(1.0+tolerance))
		throw SX::Kernel::Error<Element>("The sum of abundances exceeds 1.0");

	if (!_isotopes.empty())
		// Check that the isotope is chemically compatible with the element (same number of protons)
		if (isotope->getNProtons() != _isotopes.begin()->first->getNProtons())
			throw SX::Kernel::Error<Element>("Invalid number of protons");

	_isotopes.insert(isotopeContentsPair(isotope,abundance));

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
	double sum=std::accumulate(std::begin(_isotopes),
			                    std::end(_isotopes),
			                    0.0,
			                    [](double previous, const isotopeContentsPair& p){return previous+p.second;});
	if (std::abs(sum-1.0)>tolerance)
		throw SX::Kernel::Error<Element>("The sum of abundances is not equal to 1.0");

	double mm(0.0);
	for (auto it=_isotopes.begin();it!=_isotopes.end();++it)
		mm += it->second * (it->first->getMolarMass());
	return mm/sum;
}

unsigned int Element::getNElectrons() const
{
	if (_isotopes.empty())
		throw SX::Kernel::Error<Element>("The element is empy");

	return _isotopes.begin()->first->getNElectrons();
}

unsigned int Element::getNProtons() const
{
	if (_isotopes.empty())
		throw SX::Kernel::Error<Element>("The element is empy");

	return _isotopes.begin()->first->getNProtons();
}

} // end namespace Chemistry

} // end namespace SX
