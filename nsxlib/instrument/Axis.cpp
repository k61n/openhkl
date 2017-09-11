#include <limits>
#include <stdexcept>

#include "Axis.h"
#include "AxisFactory.h"
#include "../utils/Units.h"
#include "../utils/YAMLType.h"

namespace nsx {

Axis* Axis::create(const YAML::Node& node)
{
    // Create an instance of the source factory
    AxisFactory* axisFactory=AxisFactory::Instance();

    // Get the axis type
    std::string axisType = node["type"].as<std::string>();

    // Fetch the axis from the factory
    Axis* axis = axisFactory->create(axisType,node);

    return axis;
}

Axis::Axis()
: _label("axis"),
  _axis(Eigen::Vector3d(0.0,0.0,1.0)),
  _offset(0.0),
  _min(-std::numeric_limits<double>::infinity()),
  _max(std::numeric_limits<double>::infinity()),
  _physical(true),
  _id(0)
{
}

Axis::Axis(const std::string& label)
: _label(label),
  _axis(Eigen::Vector3d(0.0,0.0,1.0)),
  _offset(0.0),
  _min(-std::numeric_limits<double>::infinity()),
  _max(std::numeric_limits<double>::infinity()),
  _physical(true),
  _id(0)
{
}

Axis::Axis(const std::string& label, const Eigen::Vector3d& axis)
: _label(label),
  _offset(0.0),
  _min(-std::numeric_limits<double>::infinity()),
  _max(std::numeric_limits<double>::infinity()),
  _physical(true),
  _id(0)
{
    setAxis(axis);
}

Axis::Axis(const Axis& other)
: _label(other._label),
  _axis(other._axis),
  _offset(other._offset),
  _min(other._min),
  _max(other._max),
  _physical(other._physical),
  _id(other._id)
{
}

Axis::Axis(const YAML::Node& node)
{
    _label = node["name"] ? node["name"].as<std::string>() : "no name";

    Eigen::Vector3d axis = node["direction"].as<Eigen::Vector3d>();
    axis.normalize();

    _axis = axis;

    UnitsManager* um = UnitsManager::Instance();

    if (node["offset"]) {
        _offset = node["offset"]["value"].as<double>();
        double units = um->get(node["offset"]["units"].as<std::string>());
        _offset *=units;
    } else {
        _offset = 0.0;
    }

    if (node["min"]) {
        _min = node["min"]["value"].as<double>();
        double units = um->get(node["min"]["units"].as<std::string>());
        _min *=units;
    } else {
        _min = -std::numeric_limits<double>::infinity();
    }

    if (node["max"]) {
        _max = node["max"]["value"].as<double>();
        double units = um->get(node["max"]["units"].as<std::string>());
        _max *=units;
    } else {
        _max = std::numeric_limits<double>::infinity();
    }

    _physical = node["physical"].as<bool>();

    _id = node["id"] ? node["id"].as<unsigned int>() : 0;
}

Axis& Axis::operator=(const Axis& other)
{
    if (this != &other) {
        _label       = other._label;
        _axis        = other._axis;
        _offset      = other._offset;
        _min         = other._min;
        _max         = other._max;
        _physical    = other._physical;
        _id          = other._id;
    }
    return *this;
}

Axis::~Axis()
{
}


void Axis::setLabel(const std::string& label)
{
    _label=label;
}

const std::string& Axis::getLabel() const
{
    return _label;
}

void Axis::setAxis(const Eigen::Vector3d& axis)
{
    if (axis.isZero())
        throw std::runtime_error("Invalid null axis for axis "+_label);
    _axis=axis;
    // Normalize the axis
    _axis.normalize();
}
const Eigen::Vector3d& Axis::getAxis() const
{
    return _axis;
}

void Axis::setId(unsigned int id)
{
    _id = id;
}

unsigned int Axis::getId() const
{
    return _id;
}

void Axis::setOffset(double offset,bool override)
{
    _offset=offset;
}

double Axis::getOffset() const
{
    return _offset;
}

void Axis::setLimits(double min, double max)
{
    _min=min;
    _max=max;
}
void Axis::setLowLimit(double min)
{
    _min=min;
}
void Axis::setHighLimit(double max)
{
    _max=max;
}
double Axis::getLowLimit() const
{
    return _min;
}
double Axis::getHighLimit() const
{
    return _max;
}

Eigen::Vector3d Axis::transform(const Eigen::Vector3d& v,double value)
{
    Eigen::Transform<double,3,Eigen::Affine> hom=getHomMatrix(value);
    return (hom*v.homogeneous());
}

void Axis::checkRange(double value)
{
    if (value<_min || value>_max)
        throw std::range_error("Axis "+_label+": value given for transformation not within limits");
    return;
}

void Axis::setPhysical(bool physical)
{
    _physical = physical;
}

bool Axis::isPhysical() const
{
    return _physical;
}

} // end namespace nsx

