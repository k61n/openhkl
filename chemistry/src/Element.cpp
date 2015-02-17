#include <algorithm>
#include <iterator>
#include <numeric>

#include "Element.h"
#include "Error.h"
#include "Isotope.h"
#include "IsotopeManager.h"

namespace SX
{

namespace Chemistry
{

Element* Element::create(const std::string& name, const std::string& symbol)
{
	Element* element=new Element(name,symbol);
	return element;
}

Element::Element(const std::string& name, const std::string& symbol) : _name(name), _isotopes()
{
	if (!symbol.empty())
	{
		// Retrieves the isotopes from the isotopes XML database whose symbol tag matches the given symbol
		IsotopeManager* mgr=IsotopeManager::Instance();
		isotopeSet isotopes=mgr->getIsotopes<std::string>("symbol",symbol);

		// If no isotopes matches the given symbol then throws
		if (isotopes.empty())
			throw SX::Kernel::Error<Element>("No isotopes match symbol "+symbol);

		// Insert the isotopes found in the isotopes internal map
		for (auto is : isotopes)
			_isotopes.insert(isotopeContentsPair(is,is->getAbundance()));
	}

}

Element::~Element()
{
}

bool Element::operator==(const Element& other) const
{
	return (_isotopes.size() == other._isotopes.size()) &&
			std::equal(_isotopes.begin(),
					   _isotopes.end(),
					   other._isotopes.begin(),
					   [] (isotopeContentsPair a, isotopeContentsPair b) { return a.first==b.first && std::abs(a.second-b.second)<0.000001;});

}

void Element::addIsotope(Isotope* isotope, double abundance)
{
	// If the element already contains the isotope, return
	auto it=_isotopes.find(isotope);
	if (it!=_isotopes.end())
		return;

	// If some isotopes have been previously added to the Element, check that the one to be added is chemcially compatible with the other ones, otherwise throws
	if (!_isotopes.empty())
		if (isotope->getNProtons() != _isotopes.begin()->first->getNProtons())
			throw SX::Kernel::Error<Element>("Invalid number of protons");

	// If the abundance is not in the interval [0,1] then throws
	if (abundance<0.0 || abundance>1.0)
		throw SX::Kernel::Error<Element>("Invalid value for abundance");

	// If the sum of af the abundances of all the isotopes building the element (+ the one of the isotope to be added) is more than 1, then throws
	double sum=std::accumulate(std::begin(_isotopes),
			                    std::end(_isotopes),
			                    abundance,
			                    [](double previous, const isotopeContentsPair& p){return previous+p.second;});
	if (sum>(1.000001))
		throw SX::Kernel::Error<Element>("The sum of abundances exceeds 1.0");

	_isotopes.insert(isotopeContentsPair(isotope,abundance));

	return;

}

void Element::addIsotope(const std::string& name)
{
	// Retrieve the isotope (from isotopes registry or XML database if not in the isotopes registry) whose name matches the given name
	IsotopeManager* mgr=IsotopeManager::Instance();
	Isotope* is=mgr->findIsotope(name);

	// Add it to the isotopes internal map with its natural abundance
	addIsotope(is,is->getAbundance());

	return;
}

void Element::addIsotope(const std::string& name, double abundance)
{
	// Retrieve the isotope (from isotopes registry or XML database if not in the isotopes registry) whose name matches the given name
	IsotopeManager* mgr=IsotopeManager::Instance();
	Isotope* is=mgr->findIsotope(name);

	// Add it to the isotopes internal map with the given abundance
	addIsotope(is,abundance);

	return;
}

const std::string& Element::getName() const
{
	return _name;
}

std::string Element::getSymbol() const
{
	if (_isotopes.empty())
		throw SX::Kernel::Error<Element>("The element is empy");

	return _isotopes.begin()->first->getSymbol();
}

unsigned int Element::getNIsotopes() const
{
	return _isotopes.size();
}

double Element::getMolarMass() const
{
	// If the sum of af the abundances of all the isotopes building the element is more than 1, then throws
	double sum=std::accumulate(std::begin(_isotopes),
			                    std::end(_isotopes),
			                    0.0,
			                    [](double previous, const isotopeContentsPair& p){return previous+p.second;});
	if (std::abs(sum-1.0)>0.000001)
		throw SX::Kernel::Error<Element>("The sum of abundances is not equal to 1.0");

	// Compute the molar mass of the Element as the abundance-weighted sum of the molar mass of the isotopes it is made of.
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

double Element::getNNeutrons() const
{
	if (_isotopes.empty())
		throw SX::Kernel::Error<Element>("The element is empy");

	// Compute the number of neutrons of the Element as the abundance-weighted sum of the number of neutrons of the isotopes it is made of.
	double nNeutrons=0.0;
	for (auto is : _isotopes)
		nNeutrons += is.second * static_cast<double>(is.first->getNNeutrons());

	return nNeutrons;
}

double Element::getIncoherentXs() const
{
	double ixs=0.0;
	for (auto is : _isotopes)
		ixs+=is.second*is.first->getXsIncoherent();
	return ixs;
}

double Element::getAbsorptionXs(double lambda) const
{
	double sxs=0.0;
	// The scattering lengths are tabulated for thermal neutrons (wavelength=1.798 ang). So we must apply a scaling.
	double fact=lambda/1.798e-10;
	for (auto is : _isotopes)
		sxs+=is.second*is.first->getXsAbsorption();
	sxs*=fact;
	return sxs;
}

void Element::print(std::ostream& os) const
{
	os<<"Element "<<_name<<std::endl;
	if (_isotopes.empty())
		os<<"Currently empty"<<std::endl;
	else
	{
		unsigned int maxSize=0;
		for (auto it : _isotopes)
			if (it.first->getName().size() > maxSize)
				maxSize=it.first->getName().size();
		os<<"Composition:"<<std::endl;
		for (auto it : _isotopes)
		{
			os<<"\t-"<<std::setw(maxSize)<<std::setiosflags(std::ios::left)<<it.first->getName()<<" --> "<<std::setiosflags(std::ios::fixed|std::ios::right)<<std::setprecision(3)<<std::setw(7)<<100.0*it.second<<" %"<<std::endl;
			std::cout<<std::resetiosflags(std::ios::right);
		}
	}
}

std::ostream& operator<<(std::ostream& os, const Element& element)
{
	element.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
