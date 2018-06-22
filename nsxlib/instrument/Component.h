/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
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

#include <map>
#include <string>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>

#include "DirectVector.h"
#include "InstrumentTypes.h"

namespace nsx {

//! \brief Interface for the components of an instrument (e.g. detector, goniometer, source ...).
class Component {
public:
    //! Default constructor
    Component()=default;
    // Copy constructor
    Component(const Component& other)=default;
    //! Constructs a component with a given name
    Component(const std::string& name);
    //! Construct a component from a property tree node
    Component(const YAML::Node& node);
    //! Virtual copy constructor
    virtual Component* clone() const=0;
    //! Destructor.
    virtual ~Component()=0;

    //! Assignment operator
    virtual Component& operator=(const Component& other)=default;

    //! Returns the name of the component.
    const std::string& name() const;
    //! Set the name of the component
    void setName(const std::string& name);

    //! Return the goniometer attached to this component
    sptrGonio gonio() const;
    //! Return true if a geometric modifier is attached
    bool hasGonio() const;
    //! Attach a modifier to the component.
    void setGonio(sptrGonio gonio);

 protected:
    //! Name of the component
    std::string _name;
    //! Pointer to the goniometer attached to the component
    sptrGonio _gonio;
    //! The position of the component at rest, i.e. not modified by the Gonio.
    DirectVector _position;
};

} // end namespace nsx
