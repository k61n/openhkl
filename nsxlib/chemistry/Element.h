/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_ELEMENT_H_
#define NSXTOOL_ELEMENT_H_

#include <map>
#include <memory>
#include <ostream>
#include <string>

#include <boost/property_tree/ptree.hpp>

namespace SX {

namespace Chemistry {

using boost::property_tree::ptree;

// Forward declarations
class Isotope;
class Element;
class ElementManager;

// Typedefs
typedef std::shared_ptr<Isotope> sptrIsotope;
typedef std::map<std::string,sptrIsotope> isotopeMap;
typedef std::map<std::string,double> contentsMap;
typedef std::pair<std::string,sptrIsotope> strToIsotopePair;
typedef std::pair<std::string,double> strToDoublePair;

class Element {

private:

	friend class ElementManager;

public:

    static std::string DatabasePath;

    static std::string DatabaseParentNode;

    static std::string DatabaseNode;

	//! Default constructor (deleted)
	Element()=delete;

	//! Constructs an Element by fetching the isotopes whose symbol match the input name
	Element(const std::string& name);

	//! Copy constructor
	Element(const Element& other)=default;

	//! Assignment operator (deleted)
	Element& operator=(const Element& other)=default;

	Element(const ptree& node);

	//! Destructor
	~Element()=default;

	//! Returns the name of this Element
	const std::string& getName() const;

	//! Returns the isotopes that builds this Element
	const isotopeMap& getIsotopes() const;
	//! Return true whether some Isotope has been provided to this Element
	bool isEmpty() const;

	//! Returns the molar mass of the element (according to its isotopes composition)
	double getMolarMass() const;
	//! Returns the number of electrons of the element
	size_t getNElectrons() const;
	//! Returns the number of neutrons of the element. It is computed as the abundance-weighted sum of the number of neutrons of the isotopes that build this Element.
	double getNNeutrons() const;
	//! Returns the incoherent cross section of this Element. It is computed as the abundance-weighted sum of the incoherent cross section of the isotopes that build this Element
	double getIncoherentXs() const;
	//! Returns the absorption cross section at a given wavelength weighted. It is computed as the abundance-weighted sum of the absorption cross section of the isotopes that build this Element
	double getAbsorptionXs(double lambda=1.798e-10) const;

	//! Returns the abundance of the isotopes that make this Element
	const contentsMap& getAbundances() const;

	//! Add a shared pointer to an Isotope to this Element
	sptrIsotope addIsotope(sptrIsotope isotope, double abundance);
	//! Add an isotope with a given name to this Element using its natural abundance
	sptrIsotope addIsotope(const std::string& name);
	//! Add an Isotope with a given name to this Element using a given abundance
	sptrIsotope addIsotope(const std::string& name, double abundance);

	//! Inserts the information about this Element to an XML parent node
	ptree writeToXML() const;

	//! Prints informations about this Element to an output stream
	void print(std::ostream& os) const;

private:

	//! The name of the element
	std::string _name;

	//! A mapping between the names of the Isotope this Element is made of and their corresponding shared pointer
	isotopeMap _isotopes;

	//! A mapping between the names of the Isotope this Element is made of and their corresponding abundances
	contentsMap _abundances;
};

std::ostream& operator<<(std::ostream& os, const Element& element);

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ELEMENT_H_ */
