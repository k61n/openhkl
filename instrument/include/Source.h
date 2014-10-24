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

#ifndef NSXTOOL_SOURCE_H_
#define NSXTOOL_SOURCE_H_

#include <string>

#include "Component.h"

namespace SX
{

namespace Instrument
{

class Source : public Component
{
public:

	// Constructors and destructor

	// Default constructor (deleted)
	Source()=delete;
	//! Copy constructor
	Source(const Source& other);
	//! Constructs a default source with a given name
	Source(const std::string& name);
	//! Destructor
	virtual ~Source();
	//! Virtual copy constructor
	Component* clone() const;

	// Operators

	//! Assignment operator
	Source& operator=(const Source& other);

	// Getters and setters
	double getWavelength() const;
	void setWavelength(double wavelength);

private:

	double _wavelength;
};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_SOURCE_H_ */
