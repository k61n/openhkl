#include <algorithm>

#include "IsotopeManager.h"
#include "Element.h"
#include "Isotope.h"

namespace SX
{

namespace Chemistry
{

std::string Element::database="elements.xml";

elementMap Element::registeredElements=elementMap();

Element* Element::buildFromDatabase(const std::string& name)
{
	auto it=registeredElements.find(name);
	if (it!=registeredElements.end())
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
			{
				Element* element;
				// if the element node has an attribute symbol then the element is built from its natural components
				boost::optional<std::string> symb = v.second.get_optional<std::string>("<xmlattr>.symbol");
				if (symb)
					element=new Element(name,symb.get());
				else
				{
					element=new Element(name);
					BOOST_FOREACH(ptree::value_type const& vv, v.second)
					{
						if (vv.first.compare("isotope")!=0)
							continue;

						std::string name=vv.second.get<std::string>("<xmlattr>.name");
						boost::optional<double> val = v.second.get_optional<double>("<xmlattr>.abundance");
						if (val)
							element->addIsotope(name,val.get());
						else
							element->addIsotope(name);
					}
				}
				auto ret=registeredElements.insert(elementPair(name,element));
				return ret.first->second;
			}
		}
	}
	throw SX::Kernel::Error<Element>("Element "+name+" is not registered in the elements database");
}

unsigned int Element::getNRegisteredElements()
{
	return registeredElements.size();
}

void Element::registerElement(Element* el)
{
	auto it=registeredElements.find(el->getName());
	if (it!=registeredElements.end())
		throw SX::Kernel::Error<Element>("The element "+el->getName()+" is already registered in the elements database");
	registeredElements.insert(elementPair(el->getName(),el));
}

Element::Element(const std::string& name, const std::string& symbol) : _name(name), _isotopes(), _abundances()
{
	isotopeSet isotopes=Isotope::getIsotopes<std::string>("symbol",symbol);

	_isotopes.reserve(isotopes.size());
	_abundances.reserve(isotopes.size());

	for (auto is : isotopes)
	{
		_isotopes.push_back(is);
		_abundances.push_back(is->getNaturalAbundance());
	}
}

Element::Element(const std::string& name) : _name(name), _isotopes(), _abundances()
{
}

Element::~Element()
{
}

bool Element::addIsotope(Isotope* isotope)
{

	if (!_isotopes.empty())
		// Check that the isotope is chemically compatible with the element (same number of protons)
		if (isotope->getNProtons() != _isotopes[0]->getNProtons())
			throw SX::Kernel::Error<Element>("Invalid number of protons.");

		// If the element already contains the isotope, return false
		auto it=std::find(_isotopes.begin(),_isotopes.end(),isotope);
		if (it!=_isotopes.end())
			return false;

	_isotopes.push_back(isotope);

	// Everything OK, return true
	return true;

}

void Element::addIsotope(const std::string& name)
{

	Isotope* is=Isotope::buildFromDatabase(name);

	if (addIsotope(is))
		_abundances.push_back(is->getNaturalAbundance());

	return;
}

void Element::addIsotope(const std::string& name, double abundance)
{
	if (abundance<0.0)
		throw SX::Kernel::Error<Element>("Negative value for isotope abundance.");

	Isotope* is=Isotope::buildFromDatabase(name);

	if (addIsotope(is))
		_abundances.push_back(abundance);

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


} // end namespace Chemistry

} // end namespace SX
