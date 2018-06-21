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

#pragma once

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "Component.h"
#include "Monochromator.h"

namespace nsx {

//! Class encapsulating the source (incoming beam)
class Source: public Component {
public:
	//! Static constructor of a monochromatic source from a yaml tree node
	static Source* create(const YAML::Node& node);
	//! Default constructor
	Source();
	//! Copy constructor
	Source(const Source& other);
	//! Constructs a default source with a given name
	Source(const std::string& name);
	//! Constructs a source from a property tree node
	Source(const YAML::Node& node);
	//! Virtual copy constructor
	Source* clone() const;
	//! Destructor
	~Source();
	//! Assignment operator
	Source& operator=(const Source& other);
	//! Returns the monochromators registered for this Source
	const std::vector<Monochromator>& monochromators() const;
	//! Returns the number of monochromators associated with this source
	int nMonochromators() const;
	//! Select a monochromator for this source
    void setSelectedMonochromator(size_t i);
	//! Return a pointer to the selected monochromator
    Monochromator& selectedMonochromator();
	//! Add a new monochromator to this source
	void addMonochromator(Monochromator mono);

protected:

	std::vector<Monochromator> _monochromators;
	size_t _selectedMonochromator;
};

} // end namespace nsx
