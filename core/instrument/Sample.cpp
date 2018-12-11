#include <stdexcept>

#include "Sample.h"

namespace nsx {

Sample* Sample::create(const YAML::Node& node)
{
    return new Sample(node);
}

Sample::Sample() : Component("sample"), _sampleShape() {}

Sample::Sample(const std::string& name) : Component(name), _sampleShape() {}

Sample::Sample(const YAML::Node& node) : Component(node) {}

Sample* Sample::clone() const
{
    return new Sample(*this);
}

Sample::~Sample() {}

void Sample::setShape(const ConvexHull& shape)
{
    _sampleShape = shape;
}

ConvexHull& Sample::shape()
{
    return _sampleShape;
}

const ConvexHull& Sample::shape() const
{
    return _sampleShape;
}


} // end namespace nsx
