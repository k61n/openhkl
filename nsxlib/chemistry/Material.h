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

#include <boost/property_tree/ptree.hpp>

namespace SX
{

namespace Chemistry
{

class Element;
class Material;
class MaterialManager;

typedef std::shared_ptr<Element> sptrElement;
typedef std::shared_ptr<Material> sptrMaterial;
typedef std::map<sptrElement,double> elementsMap;
typedef std::map<sptrMaterial,double> materialsMap;

namespace property_tree=boost::property_tree;

//! The different modes that can be used to build a Material with its components
enum class BuildingMode : unsigned int {MassFraction=0,MolarFraction=1,Stoichiometry=2,PartialPressure=3};

class Material
{

public:
	//! A lookup between the enum FillingMode and its corresponding string representation
	static std::map<BuildingMode,std::string> buildingModeToString;
	//! A lookup between the string representation of the enum FillingMode and its corresponding FillingMode
	static std::map<std::string,BuildingMode> stringToBuildingMode;

private:

	friend class MaterialManager;

private:

	//! Returns a shared pointer to a Material.
	//! If a chemical symbol is given the element will be built from its natural isotopes otherwise it is empty and will have to be filled later by addIsotope method.
	static Material* create(const std::string& name, BuildingMode mode);

public:

	//! Destructor
	~Material();

	//! Return true if two Material objects are the same (same elements with the same contents)
	bool operator==(const Material& other) const;

	//! Returns the name of this Material
	const std::string& getName() const;

	//! Returns the building mode of this Material
	BuildingMode getBuildingMode() const;

	//! Returns the mass density of this Material
	double getMassDensity() const;
	//! Sets the mass density of this Material
	void setMassDensity(double massDensity);

	//! Returns the temperature of this Material
	double getTemperature() const;
	//! Sets the temperature of this Material
	void setTemperature(double temperature);

	//! Add an Element with a given amount to this Material
	void addElement(sptrElement element, double amount);
	//! Add an Element with a given amount to this Material.
	void addElement(const std::string& name, double amount);
	//! Add a Material with a given amount to this Material
	void addMaterial(sptrMaterial material, double amount);

	//! Returns the molar mass of this Material
	double getMolarMass() const;

	//! Returns the mass fractions of this Material
	elementsMap getMassFractions() const;
	//! Returns the molar fractions of this Material
	elementsMap getMolarFractions() const;

	//! Returns the number of atoms per volume units per element (1/m3)
	elementsMap getNAtomsPerVolume() const;
	//! Returns the number of electrons per volume units per element (1/m3)
	elementsMap getNElectronsPerVolume() const;
	//! Returns the total number of atoms per volume units (1/m3)
	double getNAtomsTotalPerVolume() const;
	//! Returns the total number of electrons per volume units (1/m3)
	double getNElectronsTotalPerVolume() const;
	//! Returns the scattering attenuation factor of this Material
	double getMuScattering() const;
	//! Returns the absorption attenuation factor of this Material
	double getMuAbsorption(double lambda=1.798e-10) const;
	//! Returns the attenuation factor of this Material
	double getMu(double lambda=1.798e-10) const;

	//! Write an XML node out of this Material and insert it into a parent node
	property_tree::ptree writeToXML() const;

	//! Print informations about this Material to an output stream
	void print(std::ostream& os) const;

private:

	//! Default constructor (deleted)
	Material()=delete;

	//! Copy constructor (deleted)
	Material(const Material& other)=delete;

	//! Constructs a Material with a given name
	Material(const std::string& name);

	//! Constructs a Material with a given name and building mode
	Material(const std::string& name, BuildingMode mode);

	//! Assignment operator (deleted)
	Material& operator=(const Material& other)=delete;

private:

	std::string _name;

	BuildingMode _buildingMode;

	double _massDensity;

	double _temperature;

	std::map<sptrElement,double> _elements;

	std::map<sptrMaterial,double> _materials;

};

std::ostream& operator<<(std::ostream& os, const Material& material);

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_MATERIAL_H_ */
