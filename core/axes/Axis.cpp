//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/axes/Axis.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <limits>
#include <stdexcept>

#include "Axis.h"
#include "AxisFactory.h"
#include "Units.h"
#include "YAMLType.h"

namespace nsx {

Axis* Axis::create(const YAML::Node& node)
{
    // Create an instance of the source factory
    AxisFactory* axisFactory = AxisFactory::Instance();

    // Get the axis type
    std::string axisType = node["type"].as<std::string>();

    // Fetch the axis from the factory
    Axis* axis = axisFactory->create(axisType, node);

    return axis;
}

Axis::Axis() : _name("axis"), _axis(Eigen::Vector3d(0.0, 0.0, 1.0)), _physical(true), _id(0) {}

Axis::Axis(const std::string& name)
    : _name(name), _axis(Eigen::Vector3d(0.0, 0.0, 1.0)), _physical(true), _id(0)
{
}

Axis::Axis(const std::string& name, const Eigen::Vector3d& axis)
    : _name(name), _physical(true), _id(0)
{
    setAxis(axis);
}

Axis::Axis(const Axis& other)
    : _name(other._name), _axis(other._axis), _physical(other._physical), _id(other._id)
{
}

Axis::Axis(const YAML::Node& node)
{
    _name = node["name"] ? node["name"].as<std::string>() : "no name";

    Eigen::Vector3d axis = node["direction"].as<Eigen::Vector3d>();
    axis.normalize();

    _axis = axis;

    _physical = node["physical"].as<bool>();

    _id = node["id"] ? node["id"].as<unsigned int>() : 0;
}

Axis& Axis::operator=(const Axis& other)
{
    if (this != &other) {
        _name = other._name;
        _axis = other._axis;
        _physical = other._physical;
        _id = other._id;
    }
    return *this;
}

Axis::~Axis() {}

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

Eigen::Vector3d Axis::transform(const Eigen::Vector3d& v, double value)
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

} // end namespace nsx
