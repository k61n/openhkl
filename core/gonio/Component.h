//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/gonio/Component.h
//! @brief     Defines class Component
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_GONIO_COMPONENT_H
#define OHKL_CORE_GONIO_COMPONENT_H

#include "core/gonio/Gonio.h"

#include <optional>

namespace ohkl {

//! Pure virtual base class for the components of an instrument (Source, Sample, Detector).

class Component {
 public:
    Component() = delete;

    virtual ~Component() = default;

    //! Construct a component from a property tree node
    Component(const YAML::Node& node);
    virtual Component* clone() const = 0;

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
    std::optional<Gonio> _gonio;
};

} // namespace ohkl

#endif // OHKL_CORE_GONIO_COMPONENT_H
