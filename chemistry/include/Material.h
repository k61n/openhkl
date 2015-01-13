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

class Element;

class Material;

typedef std::pair<Element*,double> elementContentsPair;
typedef std::map<Element*,double> elementContentsMap;
typedef std::pair<std::string,Material*> materialPair;
typedef std::map<std::string,Material*> materialMap;

class Material
{

public:

	enum class FillingMode : unsigned int {MassFraction=0,MoleFraction=1,NumberOfAtoms=2};

	enum class State : unsigned int {Solid=0,Liquid=1,Gaz=2};

public:

	//! Returns a pointer to a Material object build from the materials database
	static Material* buildFromDatabase(const std::string& name);

	//! Returns the number of registered materials
	static unsigned int getNRegisteredMaterials();

	//! Returns true if a Material with a given name has been registered
	bool hasMaterial(const std::string& name);

private:

	//! Returns a pointer to a Material object built from an XML node
	static Material* readMaterial(const ptree& node);

private:

	static std::map<std::string,State> _toState;
	static std::map<std::string,FillingMode> _toFillingMode;
	static double tolerance;
	static std::string database;
	static materialMap registry;

public:

	//! Default constructor (deleted)
	Material()=delete;

	//! Copy constructor (deleted)
	Material(const Material& other)=delete;

	//! Constructs a empty Material to be filled further with addElement and/or addMaterial methods
	Material(const std::string& name, State state=State::Solid, FillingMode fillingMode=FillingMode::MassFraction);

	//! Destructor
	~Material();

	//! Returns true if two Materials are the same (same density, state and mass fractions)
	bool operator==(const Material& other) const;

	//! Add an Element to this Material
	void addElement(Element* element, double fraction);

	//! Add an Element to this Material. The Element will be built from the elements database
	void addElement(const std::string& name, double fraction);

	//! Add a Material to this Material (that will be a composite of materials)
	void addMaterial(Material* material, double fraction);

	//! Add a Material to this Material (that will be a composite of materials). The Material will be built from the materials database
	void addMaterial(const std::string& name, double fraction);

	//! Returns the name of this Material
	const std::string& getName() const;

	//! Returns the number of elements of this Material
	unsigned int getNElements() const;

	//! Returns the density of this Material
	double getDensity() const;

	//! Set the density of this Material
	void setDensity(double density);

	//! Set the density of this Material from pressure and temperature values. Only applies for Material in Gaz state
	void setDensity(double pressure, double temperature);

	//! Returns a map of the mole fractions per element
	elementContentsMap getMoleFractions() const;

	//! Returns a map of the mass fractions per element
	elementContentsMap getMassFractions() const;

	//! Returns a map of the number of atoms per volume units per element (1/m3)
	elementContentsMap getNAtomsPerVolume() const;

	//! Returns a map of the number of electrons per volume units per element (1/m3)
	elementContentsMap getNElectronsPerVolume() const;

	//! Returns the total number of atoms per volume units (1/m3)
	double getNAtomsTotalPerVolume() const;

	//! Returns the total number of electrons per volume units (1/m3)
	double getNElectronsTotalPerVolume() const;

private:

	//! Register a Material object
	void registerMaterial(Material* material);

private:

	std::string _name;
	double _density;
	State _state;
	FillingMode _fillingMode;
	elementContentsMap _elements;

};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_MATERIAL_H_ */
