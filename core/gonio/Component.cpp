//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/gonio/Component.cpp
//! @brief     Implements class Component
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <iostream>

#include "base/utils/Units.h"
#include "base/utils/YAMLType.h"
#include "core/gonio/Component.h"
#include "core/gonio/Gonio.h"
#include "core/raw/DataKeys.h"

namespace nsx {

Component::Component(const std::string& name) : _name(name), _gonio() { }

Component::Component(const YAML::Node& node)
{
    // Sets the component name
    _name = node[nsx::ym_componentName].as<std::string>();

    _gonio = node[nsx::ym_goniometer] ? Gonio(node[nsx::ym_goniometer]) : Gonio();
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

} // namespace nsx
