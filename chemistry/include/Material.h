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

#ifndef NSXTOOL_MATERIAL_H_
#define NSXTOOL_MATERIAL_H_

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include <boost/property_tree/ptree.hpp>

namespace SX
{

namespace Chemistry
{

// Forward declarations
class Element;
class Material;

// Typedefs
typedef std::shared_ptr<Element> sptrElement;
typedef std::shared_ptr<Material> sptrMaterial;
typedef std::map<std::string,sptrElement> elementsMap;
typedef std::map<std::string,double> contentsMap;
typedef std::pair<std::string,sptrElement> strToElementPair;
typedef std::pair<std::string,double> strToDoublePair;

// Namespaces
namespace property_tree=boost::property_tree;

class Material
{

public:

	//! Enumerates the different modes that can be used to fill a Material with its components
	enum class FillingMode : unsigned int {MassFraction=0,MoleFraction=1,NumberOfAtoms=2,PartialPressure=3};

	//! Enumerates the different chemical states that can be assigned to a Material
	enum class State : unsigned int {Solid=0,Liquid=1,Gaz=2};

public:

	//! Constructs an empty Material in a given state to be filled later with a given filling mode
	static sptrMaterial create(const std::string& name, State state=State::Solid, FillingMode fillingMode=FillingMode::MassFraction);

	//! Constructs a material from a chemical formula and a given physical state
	static sptrMaterial fromChemicalFormula(std::string formula, State state=State::Solid);

public:

	//! A lookup between the enum State and its corresponding string representation
	static std::map<State,std::string> s_fromState;
	//! A lookup between the string representation of the enum State and its corresponding State
	static std::map<std::string,State> s_toState;
	//! A lookup between the enum FillingMode and its corresponding string representation
	static std::map<FillingMode,std::string> s_fromFillingMode;
	//! A lookup between the string representation of the enum FillingMode and its corresponding FillingMode
	static std::map<std::string,FillingMode> s_toFillingMode;

public:

	//! Default constructor (deleted)
	Material()=delete;

	//! Copy constructor (deleted)
	Material(const Material& other)=delete;

	//! Destructor
	~Material();

	//! Assignment operator (deleted)
	Material& operator=(const Material& other)=delete;

	//! Return true if two Material objects are the same (same density, same chemical state and same elements with the same mass fraction)
	bool operator==(const Material& other) const;

	//! Returns a shared pointer to the Element of this Material corresponding to this name. If no element of this Material matches this name, throws.
	sptrElement operator[](const std::string& name);

	//! Add a shared pointer to a Material object to this Material
	void addMaterial(sptrMaterial material, double fraction);
	//! Add an Element to this Material.
	void addElement(const std::string& name, double fraction);
	//! Add a shared pointer to an Element object to this Material
	void addElement(sptrElement element, double fraction);

	//! Returns the number of elements of this Material
	unsigned int getNElements() const;

	//! Returns the name of this Material
	const std::string& getName() const;

	//! Returns the chemical state of this Material
	State getState() const;

	//! Returns the filling mode used for adding component to this Material
	FillingMode getFillingMode() const;

	//! Returns a map of the mole fractions per element
	contentsMap getMoleFractions() const;
	//! Returns a map of the mass fractions per element
	contentsMap getMassFractions() const;

	//! Returns the density of this Material
	double getDensity() const;
	//! Sets the density of this Material
	void setDensity(double density);
	//! Returns the temperature of this Material
	double getTemperature() const;
	//! Sets the temperature of this Material
	void setTemperature(double temperature);

	//! Returns a map of the number of atoms per volume units per element (1/m3)
	contentsMap getNAtomsPerVolume() const;
	//! Returns a map of the number of electrons per volume units per element (1/m3)
	contentsMap getNElectronsPerVolume() const;
	//! Returns the total number of atoms per volume units (1/m3)
	double getNAtomsTotalPerVolume() const;
	//! Returns the total number of electrons per volume units (1/m3)
	double getNElectronsTotalPerVolume() const;
	//! Returns the attenuation factor of this Material
	double getMu(double lambda=1.798e-10) const;

	//! Print informations about this Material to an output stream
	void print(std::ostream& os) const;

	//! Inserts the information about this Material to an XML parent node
	void writeToXML(property_tree::ptree& parent) const;

private:

	//! Constructs an empty Material in a given state and filling mode (to be filled later by addElement and/or addMaterial methods)
	Material(const std::string& name, State state=State::Solid, FillingMode fillingMode=FillingMode::MassFraction);

private:

	//! The name of this Material
	std::string _name;
	//! The density of this Material
	double _density;
	//! The temperature of this Material
	double _temperature;
	//! The physical state of this Material
	State _state;
	//! The way Element or Material objects will be added to this Material
	FillingMode _fillingMode;
	//! A lookup between the name of the Element this Material is made of and their corresponding pointer
	elementsMap _elements;
	//! A lookup between the name of the Element this Material is made of and their corresponding contents. The value depends on the filling mode.
	contentsMap _contents;

};

std::ostream& operator<<(std::ostream& os, const Material& material);

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_MATERIAL_H_ */
