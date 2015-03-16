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

#ifndef NSXTOOL_IMATERIAL_H_
#define NSXTOOL_IMATERIAL_H_

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
class IMaterial;
class MaterialManager;

// Typedefs
typedef std::shared_ptr<Element> sptrElement;
typedef std::shared_ptr<IMaterial> sptrMaterial;
typedef std::map<std::string,sptrElement> strToElementMap;
typedef std::pair<std::string,sptrElement> strToElementPair;
typedef std::map<std::string,double> strToDoubleMap;
typedef std::pair<std::string,double> strToDoublePair;

// Namespaces
namespace property_tree=boost::property_tree;

// Enumerates
//! The different modes that can be used to build a Material with its components
enum class BuildingMode : unsigned int {MassFractions=0,MolarFractions=1,Stoichiometry=2,PartialPressures=3};
//! The different chemical states that can be assigned to a Material
enum class ChemicalState : unsigned int {Solid=0,Liquid=1,Gaz=2,Unknown=3};

class IMaterial
{

private:

	friend class MaterialManager;

public:

	//! A lookup between the enum State and its corresponding string representation
	static std::map<ChemicalState,std::string> stateToStr;
	//! A lookup between the string representation of the enum State and its corresponding State
	static std::map<std::string,ChemicalState> strToState;
	//! A lookup between the enum FillingMode and its corresponding string representation
	static std::map<BuildingMode,std::string> buildingModeToStr;
	//! A lookup between the string representation of the enum FillingMode and its corresponding FillingMode
	static std::map<std::string,BuildingMode> strToBuildingMode;

public:

	//! Destructor
	virtual ~IMaterial();

	//! Return true if two Material objects are the same (same density, same chemical state and same elements with the same contents)
	bool operator==(const IMaterial& other) const;
	//! If name matches one the element names of this Material, return a shared pointer to this Element. Otherwise throws.
	sptrElement operator[](const std::string& name);

	const std::string& getName() const;

	//! Get the building mode of this Material
	virtual BuildingMode getBuildingMode() const=0;

	//! Returns the chemical state of this Material
	ChemicalState getState() const;
	//! Returns a string version of the chemical state of this Material
	std::string getStateString() const;

	//! Returns the number of Elements of this Material
	unsigned int getNElements() const;

	//! Gets the mass density of this Material
	virtual double getMassDensity() const=0;
	//! Sets the mass density of this Material
	virtual void setMassDensity(double massDensity)=0;

	//! Gets the temperature of this Material
	double getTemperature() const;
	//! Sets the temperature of this Material
	void setTemperature(double temperature);

	//! Add an Element to this Material.
	virtual void addElement(const std::string& name, double)=0;
	//! Add a shared pointer to an Element object to this Material
	virtual void addElement(sptrElement element, double)=0;
	//! Add a shared pointer to a Material object to this Material
	virtual void addMaterial(sptrMaterial material, double)=0;

	//! Gets the mass fractions of this Material
	virtual strToDoubleMap getMassFractions() const=0;
	//! Gets the molar fractions of this Material
	virtual strToDoubleMap getMolarFractions() const=0;
	//! Gets the partial pressures of this Material
	virtual strToDoubleMap getPartialPressures() const=0;
	//! Gets the stochiometry of this Material
	virtual strToDoubleMap getStoichiometry() const=0;

	//! Returns the number of atoms per volume units per element (1/m3)
	strToDoubleMap getNAtomsPerVolume() const;
	//! Returns the number of electrons per volume units per element (1/m3)
	strToDoubleMap getNElectronsPerVolume() const;
	//! Returns the total number of atoms per volume units (1/m3)
	double getNAtomsTotalPerVolume() const;
	//! Returns the total number of electrons per volume units (1/m3)
	double getNElectronsTotalPerVolume() const;
	//! Returns the attenuation factor of this Material
	double getMu(double lambda=1.798e-10) const;

	//! Inserts the information about this Material to an XML parent node
	virtual void writeToXML(property_tree::ptree& parent) const=0;

	//! Print informations about this Material to an output stream
	virtual void print(std::ostream& os) const=0;

protected:

	//! Default constructor (deleted)
	IMaterial()=delete;

	//! Copy constructor (deleted)
	IMaterial(const IMaterial& other)=delete;

	//! Constructs a Material with a given name
	IMaterial(const std::string& name);

	//! Constructs a Material with a given name and chemical state
	IMaterial(const std::string& name, ChemicalState state);

	//! Constructs a Material with a given name and chemical state given in its string version
	IMaterial(const std::string& name, const std::string& state);

	//! Assignment operator (deleted)
	IMaterial& operator=(const IMaterial& other)=delete;

protected:

	//! The name of this Material
	std::string _name;
	//! The chemical state of this Material
	ChemicalState _state;
	//! The mass density of this Material
	double _massDensity;
	//! The temperature of this Material
	double _temperature;
	//! The Elements that makes this Material
	strToElementMap _elements;
	//! The contents of the Elements that makes this Material
	strToDoubleMap _contents;
};

std::ostream& operator<<(std::ostream& os, const IMaterial& material);

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_IMATERIAL_H_ */
