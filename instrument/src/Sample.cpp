#include "Sample.h"

namespace SX
{

namespace Instrument
{

Sample::Sample() : Component("sample")
{
}

Sample::Sample(const Sample& other) : Component(other)
{
}

Sample::Sample(const std::string& name) : Component(name)
{
}

Sample::~Sample()
{
}

Sample& Sample::operator=(const Sample& other)
{
	if (this != &other)
	{
		Component::operator=(other);
	}
	return *this;
}

Component* Sample::clone() const
{
	return new Sample(*this);
}

SX::Geometry::ConvexHull<double>& Sample::getShape()
{
	return _sampleShape;
}

}
} /* namespace SX */
