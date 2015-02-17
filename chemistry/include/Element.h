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
#include <ostream>
#include <string>

namespace SX
{

namespace Chemistry
{

// Forward declarations
class Isotope;
class Element;

// Typedefs
typedef std::pair<Isotope*,double> isotopeContentsPair;
typedef std::map<Isotope*,double> isotopeContentsMap;

class Element
{

public:

	//! Constructs an Element.
	//! If a chemical symbol is given the element will be built from its natural isotopes otherwise it is empty and will have to be filled later by addIsotope method.
	static Element* create(const std::string& name, const std::string& symbol="");

public:

	//! Default constructor (deleted)
	Element()=delete;

	//! Copy constructor (deleted)
	Element(const Element& other)=delete;

	//! Destructor
	~Element();

	//! Assignment operator (deleted)
	Element& operator=(const Element& other)=delete;

	//! Return true if two Elements are the same (same isotopes with the same abundances)
	bool operator==(const Element& other) const;

	//! Returns the name of this Element
	const std::string& getName() const;

	//! Returns the symbol of this Element
	std::string getSymbol() const;

	//! Returns the number of isotopes that build this Element
	unsigned int getNIsotopes() const;

	//! Returns the molar mass of the element (according to its isotopes composition)
	double getMolarMass() const;

	//! Returns the number of protons of the element
	unsigned int getNProtons() const;

	//! Returns the number of electrons of the element
	unsigned int getNElectrons() const;

	//! Returns the number of neutrons of the element. It is computed as the abundance-weighted sum of the number of neutrons of the isotopes that build this Element.
	double getNNeutrons() const;

	//! Returns the incoherent cross section of this Element. It is computed as the abundance-weighted sum of the incoherent cross section of the isotopes that build this Element
	double getIncoherentXs() const;

	//! Returns the absorption cross section at a given wavelength weighted. It is computed as the abundance-weighted sum of the absorption cross section of the isotopes that build this Element
	double getAbsorptionXs(double lambda=1.798e-10) const;

	//! Prints informations about this Element to an output stream
	void print(std::ostream& os) const;

	//! Add an isotope to this Element using its natural abundance
	void addIsotope(const std::string& name);

	//! Add an isotope to this Element using a given abundance
	void addIsotope(const std::string& name, double abundance);

private:

	//! Constructs an Element by fetching the isotopes from the isotopes database whose symbol matches the given symbol
	Element(const std::string& name, const std::string& symbol);

	//! Add a pointer to an Isotope object with a given abudance to this Element
	void addIsotope(Isotope* isotope, double abundance);

private:
	//! The name of the element
	std::string _name;

	//! A map that stores the abundance of each isotope that builds this Element
	isotopeContentsMap _isotopes;
};

std::ostream& operator<<(std::ostream& os, const Element& element);

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ELEMENT_H_ */
