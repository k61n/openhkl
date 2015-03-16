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

#ifndef NSXTOOL_MATERIALFROMSTOICHIOMETRY_H_
#define NSXTOOL_MATERIALFROMSTOICHIOMETRY_H_

#include "Element.h"
#include "IMaterial.h"

namespace SX
{

namespace Chemistry
{

class MaterialFromStoichiometry: public IMaterial
{
public:

	//! Constructs and returns a pointer to an empty Material with a given name and chemical state
	static IMaterial* create(const std::string& name, ChemicalState state);

public:

	~MaterialFromStoichiometry();

	//! Get the building mode of this Material
	BuildingMode getBuildingMode() const;

	//! Gets the density of this Material
	double getMassDensity() const;
	//! Sets the density of this Material
	void setMassDensity(double density);

	//! Add an Element to this Material.
	void addElement(const std::string& name, double stoichiometry);
	//! Add a shared pointer to an Element object to this Material
	void addElement(sptrElement element, double stoichiometry);
	//! Add a shared pointer to a Material object to this Material
	void addMaterial(sptrMaterial material, double stoichiometry);

	//! Gets the mass fractions of this Material
	strToDoubleMap getMassFractions() const;
	//! Gets the molar fractions of this Material
	strToDoubleMap getMolarFractions() const;
	//! Gets the partial pressures of this Material
	strToDoubleMap getPartialPressures() const;
	//! Gets the Stoichiometry of this Material
	strToDoubleMap getStoichiometry() const;

	//! Print informations about this Material to an output stream
	void print(std::ostream& os) const;

	//! Inserts the information about this Material to an XML parent node
	void writeToXML(property_tree::ptree& parent) const;

private:

	//! Constructs an empty Material with a given name
	MaterialFromStoichiometry(const std::string& name);

	//! Constructs an empty Material with a given name and chemical state
	MaterialFromStoichiometry(const std::string& name, ChemicalState state);

	//! Constructs an empty Material with a given name and chemical state in its string version
	MaterialFromStoichiometry(const std::string& name, const std::string& strState);

	//! Default constructor (deleted)
	MaterialFromStoichiometry()=delete;

	//! Copy constructor (deleted)
	MaterialFromStoichiometry(const MaterialFromStoichiometry& other)=delete;

	//! Assignment operator (deleted)
	MaterialFromStoichiometry& operator=(const MaterialFromStoichiometry& other)=delete;};
} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_MATERIALFROMSTOICHIOMETRY_H_ */
