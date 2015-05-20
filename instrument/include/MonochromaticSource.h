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

#ifndef NSXTOOL_MONOCHROMATICSOURCE_H_
#define NSXTOOL_MONOCHROMATICSOURCE_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

#include "Source.h"

namespace SX
{

namespace Instrument
{

class MonochromaticSource : public Source
{
public:

	//! Static constructor of a monochromatic source from a property tree node
	static Source* create(const proptree::ptree& node);

	// Default constructor
	MonochromaticSource();
	//! Copy constructor
	MonochromaticSource(const MonochromaticSource& other);
	//! Constructs a default source with a given name
	MonochromaticSource(const std::string& name);
	//! Constructs a monochromatic source from a property tree node
	MonochromaticSource(const proptree::ptree& node);
	//! Destructor
	~MonochromaticSource();
	//! Virtual copy constructor
	Source* clone() const;

	//! Assignment operator
	MonochromaticSource& operator=(const MonochromaticSource& other);

	//! Get the wavelength of the monochromatic source
	double getWavelength() const;
	//! Set the wavelength of the monochromatic source
	void setWavelength(double wavelength);

	//! Get the incoming wave vector
	Eigen::Vector3d getKi() const;

private:
	double _wavelength;

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_MONOCHROMATICSOURCE_H_ */
