//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/gonio/Component.cpp
//! @brief     Implements class Component
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/gonio/Component.h"
#include "base/utils/Units.h"
#include "base/utils/YAMLType.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

Component::Component(const YAML::Node& node)
{
    _name = node[ym_componentName].as<std::string>();
    _gonio = {};
    if (node[ym_goniometer])
        _gonio = {node[ym_goniometer]};
}

const Gonio& Component::gonio() const
{
    return _gonio.value();
}

Gonio& Component::gonio()
{
    return _gonio.value();
}

const std::string& Component::name() const
{
    return _name;
}

void Component::setName(const std::string& name)
{
    _name = name;
}

} // namespace ohkl
