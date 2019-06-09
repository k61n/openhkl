//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/gonio/Component.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_GONIO_COMPONENT_H
#define CORE_GONIO_COMPONENT_H




#include "core/geometry/DirectVector.h"
#include "core/gonio/Gonio.h"

namespace nsx {

//! \brief Interface for the components of an instrument (e.g. detector,
//! goniometer, source ...).
class Component {

public:
    Component() = default;
    Component(const Component& other);

    //! Constructs a component with a given name
    Component(const std::string& name);

    //! Construct a component from a property tree node
    Component(const YAML::Node& node);
    virtual Component* clone() const = 0;

    //! Destructor.
    virtual ~Component() = 0;
    virtual Component& operator=(const Component& other);

    //! Returns the name of the component.
    const std::string& name() const;

    //! Sets the name of the component
    void setName(const std::string& name);

    //! Returns a const reference to the goniometer
    const Gonio& gonio() const;

    //! Returns a non-const reference to the goniometer
    Gonio& gonio();

protected:
    //! Name of the component
    std::string _name;

    //! The goniometer attached to the component
    Gonio _gonio;

    //! The position of the component at rest, i.e. not modified by the Gonio.
    DirectVector _position;
};

} // namespace nsx

#endif // CORE_GONIO_COMPONENT_H
