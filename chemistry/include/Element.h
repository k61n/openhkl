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
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace SX
{

namespace Chemistry
{

using boost::property_tree::ptree;

class Isotope;

class Element;

typedef std::pair<Isotope*,double> isotopeContentsPair;
typedef std::map<Isotope*,double> isotopeContentsMap;
typedef std::pair<std::string,Element*> elementPair;
typedef std::map<std::string,Element*> elementMap;

class Element
{

public:

	//! Returns an pointer to an Element object built from the elements database
	static Element* buildFromDatabase(const std::string& name);

	//! Returns the number of registered elements
	static unsigned int getNRegisteredElements();

	static Element* readElement(const ptree& node);

	static bool hasElement(const std::string& name);

private:

	static double tolerance;
	static std::string database;
	static elementMap registry;

public:

	//! Default constructor (deleted)
	Element()=delete;

	//! Copy constructor (deleted)
	Element(const Element& other)=delete;

	//! Constructs an Element fetching the isotope of the isotopes database whose symbol is |symbol|
	Element(const std::string& name, const std::string& symbol);

	//! Constructs an empty Element to be filled further with addIsotope method
	Element(const std::string& name);

	//! Destructor
	~Element();

	//! Return true if two Elements are the same (same isotopes with the same abundances)
	bool operator==(const Element& other) const;

	//! Assignment operator (deleted)
	Element& operator=(const Element& other)=delete;

	//! Add a pointer to an Isotope object with a given abudance to this Element
	void addIsotope(Isotope* isotope, double abundance);

	//! Add an isotope to this Element using its natural abundance
	void addIsotope(const std::string& name);

	//! Add an isotope to this Element using a given abundance
	void addIsotope(const std::string& name, double abundance);

	//! Returns the name of this Element
	const std::string& getName() const;

	//! Returns the number of isotopes this Element is made of
	unsigned int getNIsotopes() const;

	//! Returns the molar mass of the element (according to its isotopes composition)
	double getMolarMass() const;

	//! Returns the number of protons of the element
	unsigned int getNProtons() const;

	//! Returns the number of electrons of the element
	unsigned int getNElectrons() const;

private:

	//! Register an Element object
	void registerElement(Element* element);

private:
	std::string _name;
	isotopeContentsMap _isotopes;
};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ELEMENT_H_ */
