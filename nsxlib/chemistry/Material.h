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

#include "IsotopeDatabaseManager.h"
#include "../utils/Types.h"

namespace nsx {

namespace property_tree=boost::property_tree;

class Material
{

public:

    //! Default constructor (deleted)
    Material()=delete;

    //! Constructs a Material with a given name
    Material(const std::string& formula);

    //! Copy constructor (deleted)
    Material(const Material& other)=default;

	//! Destructor
	~Material()=default;

    //! Assignment operator (deleted)
    Material& operator=(const Material& other)=delete;

//	//! Return true if two Material objects are the same (same elements with the same contents)
//	bool operator==(const Material& other) const;

	//! Returns the name of this Material
	const std::string& formula() const;

	const isotopeContents& isotopes() const;

	//! Returns the mass density of this Material
	double massDensity() const;
	//! Sets the mass density of this Material
	void setMassDensity(double massDensity);

	//! Returns the molar mass of this Material
	double molarMass() const;

	//! Returns the mass fractions of this Material
    isotopeContents massFractions() const;

	//! Returns the number of atoms per volume units per element (1/m3)
    isotopeContents atomicNumberDensity() const;

	//! Returns the scattering attenuation factor of this Material
	double muIncoherent() const;
	//! Returns the absorption attenuation factor of this Material at a given wavelength
	double muAbsorption(double lambda=1.798e-10) const;

	//! Print informations about this Material to an output stream
	void print(std::ostream& os) const;

private:

	std::string _formula;

	isotopeContents _isotopes;

	double _massDensity;
};

#ifndef SWIG
std::ostream& operator<<(std::ostream& os, const Material& material);
#endif

} // end namespace nsx

#endif /* NSXTOOL_MATERIAL_H_ */
