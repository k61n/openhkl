#include "Sample.h"

namespace SX
{

namespace Instrument
{

Sample::Sample() : Component("sample"), _sampleShape(), _material()
{
}

Sample::Sample(const Sample& other) : Component(other), _sampleShape(other._sampleShape), _material(other._material)
{
}

Sample::Sample(const std::string& name) : Component(name), _sampleShape(), _material()
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
		_sampleShape=other._sampleShape;
		_material=other._material;
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

SX::Chemistry::sptrMaterial Sample::getMaterial() const
{
	return _material;
}

} // end namespace Instrument

} /* namespace SX */
