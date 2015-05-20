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

#ifndef NSXTOOL_POLYCHROMATICSOURCE_H_
#define NSXTOOL_POLYCHROMATICSOURCE_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

#include "Source.h"

namespace SX
{

namespace Instrument
{

class PolychromaticSource : public Source
{
public:

	//! Static constructor of a polychromatic source from a property tree node
	static Source* create(const proptree::ptree& node);

	// Default constructor
	PolychromaticSource();
	//! Copy constructor
	PolychromaticSource(const PolychromaticSource& other);
	//! Constructs a default source with a given name
	PolychromaticSource(const std::string& name);
	//! Constructs a polychromatic source from a property tree node
	PolychromaticSource(const proptree::ptree& node);
	//! Destructor
	~PolychromaticSource();
	//! Virtual copy constructor
	Source* clone() const;

	//! Assignment operator
	PolychromaticSource& operator=(const PolychromaticSource& other);

	//! Get the average wavelength of the polychromatic source
	double getWavelength() const;
	//! Get the minimum wavelength of the polychromatic source
	double getWavelengthMin() const;
	//! Get the maximum wavelength of the polychromatic source
	double getWavelengthMax() const;
	//! Set the minimum and maximum wavelength of the polychromatic source to the same value
	void setWavelength(double wavelength);
	//! Set the minimum wavelength of the polychromatic source
	void setWavelengthMin(double wavelength);
	//! Set the maximum wavelength of the polychromatic source
	void setWavelengthMax(double wavelength);

	//! Get the incoming average wave vector
	Eigen::Vector3d getKi() const;
	//! Get the incoming wave vector corresponding to the minimum wavelength
	Eigen::Vector3d getKiMin() const;
	//! Get the incoming wave vector corresponding to the maximum wavelength
	Eigen::Vector3d getKiMax() const;

private:

	double _wavelengthMin;
	double _wavelengthMax;

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_POLYCHROMATICSOURCE_H_ */
