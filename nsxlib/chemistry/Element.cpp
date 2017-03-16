#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>

#include "Element.h"
#include "Isotope.h"
#include "ChemicalDatabaseManager.h"
#include "../utils/Path.h"
#include "../utils/Units.h"

namespace SX {

namespace Chemistry {

using SX::Units::UnitsManager;
using SX::Utils::Path;

std::string Element::DatabasePath = Path::getDataBasesPath("elements");

std::string Element::DatabaseParentNode = "elements";

std::string Element::DatabaseNode = "element";

Element::Element(const std::string& name) : _name(name), _isotopes()
{
	ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();
    const auto& isotopeDatabase=imgr->getDatabase();

    auto it=isotopeDatabase.find(name);

    // Case of a "pure" isotope (e.g. H[1],C[14])
    if (it != isotopeDatabase.end()) {
		sptrIsotope isotope=imgr->getChemicalObject(name);
		addIsotope(isotope,1.0);
    } else {
	    for (const auto& isotope : isotopeDatabase) {

	        std::string symbolName = isotope.second->getProperty<std::string>("symbol");

	        if (symbolName == name) {
	            addIsotope(isotope.second,isotope.second->getProperty<double>("natural_abundance"));
	        }
	    }
    }
}

Element::Element(const ptree& node)
{
	_name=node.get<std::string>("<xmlattr>.name");

    // Loop over the 'isotope' nodes
    for (const auto& subnode : node) {

    	if (subnode.first.compare("isotope")!=0)
            continue;

        std::string isotopeName=subnode.second.get<std::string>("<xmlattr>.name");

        ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();
    	sptrIsotope isotope=imgr->getChemicalObject(isotopeName);

    	double abundance = subnode.second.get<double>("abundance",isotope->getProperty<double>("natural_abundance"));
		addIsotope(isotope,abundance*0.01);
    }
}

sptrIsotope Element::addIsotope(sptrIsotope isotope, double abundance)
{

	// If the abundance is not in the interval [0,1] then throws
	if (abundance<0.0 || abundance>1.0)
		throw std::runtime_error("Element "+_name+": invalid value for abundance ("+std::to_string(abundance)+")");

	std::string name=isotope->getName();

	static std::string symbol;
	if (_isotopes.empty())
	{
		_isotopes.insert(std::make_pair(name,isotope));
		_abundances.insert(std::make_pair(name,abundance));
		symbol=isotope->getProperty<std::string>("symbol");
	}
	else
	{
		auto it=_isotopes.find(name);
		if (it!=_isotopes.end())
			_abundances[name] = abundance;
		else
		{
			if (symbol.compare(isotope->getProperty<std::string>("symbol")) != 0)
				throw std::runtime_error("Element "+_name+" is made of isotopes of different chemical species.");

			_isotopes.insert(std::make_pair(name,isotope));
			_abundances.insert(std::make_pair(name,abundance));
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

bool Element::isEmpty() const
{
	return _isotopes.empty();
}

double Element::getMolarMass() const
{
	// Compute the molar mass of the Element as the abundance-weighted sum of the molar mass of the isotopes it is made of.
	double molarMass(0.0);
	double sumAbundances(0.0);
	for (const auto& p : _abundances) {
		molarMass += p.second * _isotopes.at(p.first)->getProperty<double>("molar_mass");
		sumAbundances += p.second;
	}
	return molarMass/sumAbundances;
}


size_t Element::getNElectrons() const
{
	if (_isotopes.empty()){
		return 0;
	}

	return _isotopes.begin()->second->getProperty<int>("n_electrons");
}

double Element::getNNeutrons() const
{
	if (_isotopes.empty()) {
		return 0.0;
	}

	// Compute the number of neutrons of the Element as the abundance-weighted sum of the number of neutrons of the isotopes it is made of.
	double nNeutrons(0.0);
	for (const auto& p : _abundances) {
		nNeutrons += p.second * static_cast<double>(_isotopes.at(p.first)->getProperty<int>("n_neutrons"));
	}

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

ptree Element::writeToXML() const
{
	ptree node;
	node.put("<xmlattr>.name",_name);
	for (const auto& is : _abundances)
	{
		ptree& isnode=node.add("isotope","");
		isnode.put("<xmlattr>.name",is.first);
		isnode.put<double>("abundance",is.second);
	}
	return node;
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
