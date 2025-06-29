//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/gonio/Axis.cpp
//! @brief     Implements class Axis
//!
//! @homepage  https://openhkl.org
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

namespace ohkl {

Axis* Axis::create(const YAML::Node& node)
{
    std::string axisType = node[ym_axisType].as<std::string>();

    return AxisFactory::instance().create(axisType, node);
}

Axis::Axis(const std::string& name, const Eigen::Vector3d& axis)
    : _name(name), _physical(true), _id(0)
{
    setAxis(axis);
}

Axis::Axis(const YAML::Node& node)
{
    _name = node[ym_axisName].as<std::string>();

    Eigen::Vector3d axis = node[ym_axisDirection].as<Eigen::Vector3d>();
    axis.normalize();

    _axis = axis;

    _physical = node[ym_axisPhysical].as<bool>();

    _id = node[ym_axisId] ? node[ym_axisId].as<unsigned int>() : 0;
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

} // namespace ohkl
