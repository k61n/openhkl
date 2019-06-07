//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Component.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <iostream>

#include "Component.h"
#include "Gonio.h"
#include "Units.h"
#include "YAMLType.h"

namespace nsx {

Component::Component(const std::string& name) : _name(name), _gonio() {}

Component::Component(const YAML::Node& node)
{
    // Set the component name
    _name = node["name"].as<std::string>();

    _gonio = node["goniometer"] ? Gonio(node["goniometer"]) : Gonio();
}

Component::Component(const Component& other)
{
    *this = other;
}

Component::~Component() {}

Component& Component::operator=(const Component& other)
{
    if (this != &other) {
        _name = other._name;
        _position = other._position;
        _gonio = Gonio(other._gonio);
    }

    return *this;
}

const Gonio& Component::gonio() const
{
    return _gonio;
}

Gonio& Component::gonio()
{
    return _gonio;
}

const std::string& Component::name() const
{
    return _name;
}

void Component::setName(const std::string& name)
{
    _name = name;
}

} // end namespace nsx
