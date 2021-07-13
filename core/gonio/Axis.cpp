//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/gonio/Axis.cpp
//! @brief     Implements class Axis
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <limits>
#include <stdexcept>

#include "base/utils/Units.h"
#include "base/utils/YAMLType.h"
#include "core/gonio/Axis.h"
#include "core/gonio/AxisFactory.h"
#include "core/raw/DataKeys.h"

namespace nsx {

Axis* Axis::create(const YAML::Node& node)
{
    std::string axisType = node[nsx::ym_axisType].as<std::string>();

    return AxisFactory::instance().create(axisType, node);
}

Axis::Axis()
    : _name(nsx::kw_axisDefaultName), _axis(Eigen::Vector3d(0.0, 0.0, 1.0)), _physical(true), _id(0)
{
}

Axis::Axis(const std::string& name)
    : _name(name), _axis(Eigen::Vector3d(0.0, 0.0, 1.0)), _physical(true), _id(0)
{
}

Axis::Axis(const std::string& name, const Eigen::Vector3d& axis)
    : _name(name), _physical(true), _id(0)
{
    setAxis(axis);
}

Axis::Axis(const YAML::Node& node)
{
    _name = node[nsx::ym_axisName] ? node[nsx::ym_axisName].as<std::string>()
                                   : nsx::kw_axisDefaultName;

    Eigen::Vector3d axis = node[nsx::ym_axisDirection].as<Eigen::Vector3d>();
    axis.normalize();

    _axis = axis;

    _physical = node[nsx::ym_axisPhysical].as<bool>();

    _id = node[nsx::ym_axisId] ? node[nsx::ym_axisId].as<unsigned int>() : nsx::kw_axisDefaultId;
}

Axis::~Axis() = default;

void Axis::setName(const std::string& name)
{
    _name = name;
}

const std::string& Axis::name() const
{
    return _name;
}

void Axis::setAxis(const Eigen::Vector3d& axis)
{
    if (axis.isZero())
        throw std::runtime_error("Invalid null axis for axis " + _name);
    _axis = axis;
    // Normalize the axis
    _axis.normalize();
}
const Eigen::Vector3d& Axis::axis() const
{
    return _axis;
}

void Axis::setId(unsigned int id)
{
    _id = id;
}

unsigned int Axis::id() const
{
    return _id;
}

Eigen::Vector3d Axis::transform(const Eigen::Vector3d& v, double value) const
{
    Eigen::Transform<double, 3, Eigen::Affine> hom = affineMatrix(value);
    return (hom * v.homogeneous());
}

void Axis::setPhysical(bool physical)
{
    _physical = physical;
}

bool Axis::physical() const
{
    return _physical;
}

std::ostream& operator<<(std::ostream& os, const Axis& axis)
{
    return axis.printSelf(os);
}

} // namespace nsx
