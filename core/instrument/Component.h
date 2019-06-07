//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Component.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_COMPONENT_H
#define CORE_INSTRUMENT_COMPONENT_H

#include <map>
#include <string>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>

#include "DirectVector.h"
#include "Gonio.h"
#include "InstrumentTypes.h"

namespace nsx {

//! \brief Interface for the components of an instrument (e.g. detector,
//! goniometer, source ...).
class Component {

public:
    //! Default constructor
    Component() = default;

    // Copy constructor
    Component(const Component& other);

    //! Constructs a component with a given name
    Component(const std::string& name);

    //! Construct a component from a property tree node
    Component(const YAML::Node& node);

    //! Virtual copy constructor
    virtual Component* clone() const = 0;

    //! Destructor.
    virtual ~Component() = 0;

    //! Assignment operator
    virtual Component& operator=(const Component& other);

    //! Returns the name of the component.
    const std::string& name() const;

    //! Set the name of the component
    void setName(const std::string& name);

    //! Return a const reference to the goniometer
    const Gonio& gonio() const;

    //! Return a non-const reference to the goniometer
    Gonio& gonio();

protected:
    //! Name of the component
    std::string _name;

    //! The goniometer attached to the component
    Gonio _gonio;

    //! The position of the component at rest, i.e. not modified by the Gonio.
    DirectVector _position;
};

} // end namespace nsx

#endif // CORE_INSTRUMENT_COMPONENT_H
