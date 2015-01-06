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

namespace SX
{

namespace Chemistry
{

class Isotope;

class Element;

typedef std::pair<std::string,Element*> elementPair;
typedef std::map<std::string,Element*> elementMap;

class Element
{

public:

	//! Returns an pointer to an Element object built from the elements database
	static Element* buildFromDatabase(const std::string& name);

	//! Returns the number of registered elements
	static unsigned int getNRegisteredElements();

	//! Register an Element in the registry
	static void registerElement(Element* el);

private:

	static std::string database;
	static elementMap registeredElements;

public:

	//! Default constructor (deleted)
	Element()=delete;

	//! Copy constructor (deleted)
	Element(const Element& other)=delete;

	//! Constructs an Element from the Isotope Manager
	Element(const std::string& name, const std::string& symbol);

	//! Constructs an empty Element
	Element(const std::string& name);

	//! Destructor
	~Element();

	//! Assignment operator (deleted)
	Element& operator=(const Element& other)=delete;

	//! Add an isotope to this Element using its natural abundance
	void addIsotope(const std::string& name);

	//! Add an isotope to this Element using a given abundance
	void addIsotope(const std::string& name, double abundance);

	//! Returns the name of this Element
	const std::string& getName() const;

	//! Returns the number of isotopes this Element is made of
	unsigned int getNIsotopes() const;

private:

	bool addIsotope(Isotope* isotope);

private:
	std::string _name;
	std::vector<Isotope*> _isotopes;
	std::vector<double> _abundances;

};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ELEMENT_H_ */
