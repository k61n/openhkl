#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>

#include "Element.h"
#include "Isotope.h"
#include "ChemicalDatabaseManager.h"
#include "../kernel/Error.h"

namespace SX {

namespace Chemistry {

Element* Element::create(const std::string& name)
{
	return (new Element(name));
}

Element::Element(const std::string& name) : _name(name), _symbol(), _isotopes(), _natural(false)
{
	std::size_t bracket=name.find_first_of("[");
	ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();
	// Case of an element which is a "pure" isotope
	if (bracket!=std::string::npos) {
		sptrIsotope isotope=imgr->getChemicalObject(name);
		addIsotope(isotope,1.0);
	}
	else {
	    const auto& isotopeDatabase=imgr->getDatabase();

	    for (const auto& isotope : isotopeDatabase) {

	        std::string symbolName = isotope.second->getProperty<std::string>("symbol");

	        if (symbolName == name) {
	            addIsotope(isotope.second,isotope.second->getProperty<double>("natural_abundance"));
	        }
	    }
	}
}

Element::~Element()
{
}

bool Element::operator==(const Element& other) const
{
	return (_abundances.size() == other._abundances.size()) &&
			std::equal(_abundances.begin(),
					   _abundances.end(),
					   other._abundances.begin(),
					   [] (strToDoublePair a, strToDoublePair b) { return a.first==b.first && std::abs(a.second-b.second)<0.000001;});

}

sptrIsotope Element::operator[](const std::string& name)
{
	try
	{
		return _isotopes.at(name);
	}
	catch(const std::out_of_range& e)
	{
		throw SX::Kernel::Error<Element>("No isotope match "+name+" name in element "+_name);
	}
}

sptrIsotope Element::addIsotope(sptrIsotope isotope, double abundance)
{

	// If the abundance is not in the interval [0,1] then throws
	if (abundance<0.0 || abundance>1.0)
		throw SX::Kernel::Error<Element>("Invalid value for abundance");

	std::string name=isotope->getName();

	if (_isotopes.empty())
	{
		_isotopes.insert(strToIsotopePair(name,isotope));
		_abundances.insert(strToDoublePair(name,abundance));
		_symbol=isotope->getProperty<std::string>("symbol");
	}
	else
	{
		auto it=_isotopes.find(name);
		if (it!=_isotopes.end())
			_abundances[name] = abundance;
		else
		{
			if (_symbol.compare(isotope->getProperty<std::string>("symbol")) != 0)
				throw SX::Kernel::Error<Element>("The element is made of isotopes of different chemical species.");

			_isotopes.insert(strToIsotopePair(name,isotope));
			_abundances.insert(strToDoublePair(name,abundance));
		}
	}
	return isotope;
}

sptrIsotope Element::addIsotope(const std::string& name)
{
	// Retrieve the isotope (from isotopes registry or XML database if not in the isotopes registry) whose name matches the given name
    ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();
	sptrIsotope isotope=imgr->getChemicalObject(name);

	// Add it to the isotopes internal map with its natural abundance
	addIsotope(isotope,isotope->getProperty<double>("natural_abundance"));

	return isotope;
}

sptrIsotope Element::addIsotope(const std::string& name, double abundance)
{
	// Retrieve the isotope (from isotopes registry or XML database if not in the isotopes registry) whose name matches the given name
    ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();
	sptrIsotope isotope=imgr->getChemicalObject(name);

	// Add it to the isotopes internal map with the given abundance
	addIsotope(isotope,abundance);

	return isotope;
}

const std::string& Element::getName() const
{
	return _name;
}

const isotopeMap& Element::getIsotopes() const
{
	return _isotopes;
}

unsigned int Element::getNIsotopes() const
{
	return _isotopes.size();
}

bool Element::isEmpty() const
{
	return _isotopes.empty();
}

double Element::getAbundance(const std::string& name) const
{
	try
	{
		return _abundances.at(name);
	}
	catch (const std::out_of_range& e)
	{
		throw SX::Kernel::Error<Element>("No isotope match "+name+" name in element "+_name);
	}
}

void Element::setAbundance(const std::string& name, double abundance)
{
	// If the abundance is not in the interval [0,1] then throws
	if (abundance<0.0 || abundance>1.0)
		throw SX::Kernel::Error<Element>("Invalid value for abundance");

	auto it=_abundances.find(name);

	if (it!=_abundances.end())
		_abundances[name]=abundance;
}

double Element::getMolarMass() const
{
	// If the sum of af the abundances of all the isotopes building the element is more than 1, then throws
	double sum=std::accumulate(std::begin(_abundances),
			                    std::end(_abundances),
			                    0.0,
			                    [](double previous, const strToDoublePair& p){return previous+p.second;});
	if (std::abs(sum-1.0)>0.000001)
		throw SX::Kernel::Error<Element>("The sum of abundances is not equal to 1.0");

	// Compute the molar mass of the Element as the abundance-weighted sum of the molar mass of the isotopes it is made of.
	double mm(0.0);
	for (auto& p : _abundances)
		mm += p.second * _isotopes.at(p.first)->getProperty<double>("molar_mass");
	return mm/sum;
}


unsigned int Element::getNElectrons() const
{
	if (_isotopes.empty())
		throw SX::Kernel::Error<Element>("The element is empy");

	return _isotopes.begin()->second->getProperty<int>("n_electrons");
}

unsigned int Element::getNProtons() const
{
	if (_isotopes.empty())
		throw SX::Kernel::Error<Element>("The element is empy");

	return _isotopes.begin()->second->getProperty<int>("n_protons");
}

double Element::getNNeutrons() const
{
	if (_isotopes.empty())
		throw SX::Kernel::Error<Element>("The element is empy");

	// Compute the number of neutrons of the Element as the abundance-weighted sum of the number of neutrons of the isotopes it is made of.
	double nNeutrons=0.0;
	for (const auto& p : _abundances)
		nNeutrons += p.second * static_cast<double>(_isotopes.at(p.first)->getProperty<int>("n_neutrons"));

	return nNeutrons;
}

double Element::getIncoherentXs() const
{
	double ixs=0.0;
	for (const auto& p : _abundances)
		ixs+=p.second*_isotopes.at(p.first)->getProperty<double>("xs_incoherent");
	return ixs;
}

double Element::getAbsorptionXs(double lambda) const
{
	double sxs=0.0;
	// The scattering lengths are tabulated for thermal neutrons (wavelength=1.798 ang). So we must apply a scaling.
	double fact=lambda/1.798e-10;
	for (const auto& p : _abundances)
		sxs+=p.second*_isotopes.at(p.first)->getProperty<double>("xs_absorption");
	sxs*=fact;
	return sxs;
}

const contentsMap& Element::getAbundances() const
{
	return _abundances;
}

property_tree::ptree Element::writeToXML() const
{
	property_tree::ptree node;
	node.put("<xmlattr>.name",_name);
	for (const auto& is : _abundances)
	{
		property_tree::ptree& isnode=node.add("isotope","");
		isnode.put("<xmlattr>.name",is.first);
		isnode.put<double>("abundance",is.second);
	}
	return node;
}

void Element::setNatural(bool natural)
{
	_natural=natural;
}

bool Element::isNatural() const
{
	return _natural;
}

const std::string& Element::getSymbol() const
{
	return _symbol;
}

void Element::print(std::ostream& os) const
{
	os<<"Name    = "<<_name<<std::endl;
	for (const auto& it : _abundances)
	{
		os<<"\t-"<<std::setiosflags(std::ios::left)<<_isotopes.at(it.first)->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<100.0*it.second<<" %"<<std::endl;
		os<<std::resetiosflags(std::ios::right);
	}
}

std::ostream& operator<<(std::ostream& os, const Element& element)
{
	element.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
