#include <stdexcept>

#include <Eigen/Dense>

#include "Gonio.h"
#include "RotAxis.h"
#include "Sample.h"
#include "TransAxis.h"
#include "UnitCell.h"

namespace nsx {

Sample* Sample::create(const YAML::Node& node)
{
    return new Sample(node);
}

Sample::Sample() : Component("sample"), _sampleShape(), _cells()
{
}

Sample::Sample(const Sample& other): Component(other), _sampleShape(other._sampleShape)
{
}

Sample::Sample(const std::string& name): Component(name), _sampleShape()
{
}

Sample::Sample(const YAML::Node& node): Component(node)
{
}

Sample* Sample::clone() const
{
    return new Sample(*this);
}

Sample::~Sample()
{
}

Sample& Sample::operator=(const Sample& other)
{
    if (this != &other) {
        Component::operator=(other);
        _sampleShape = other._sampleShape;
    }
    return *this;
}

void Sample::setShape(const ConvexHull& shape)
{
    _sampleShape = shape;
}

ConvexHull& Sample::shape()
{
    return _sampleShape;
}

const UnitCellList& Sample::unitCells() const
{
    return _cells;
}

UnitCellList& Sample::unitCells()
{
    return _cells;
}

} // end namespace nsx
