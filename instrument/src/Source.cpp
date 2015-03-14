#include "Source.h"

namespace SX
{

namespace Instrument
{

Source::Source() : Component("source"), _wavelength(1.0),_offset(0.0),_offsetFixed(true)
{
}

Source::Source(const Source& other) : Component(other), _wavelength(other._wavelength),_offset(other._offset),_offsetFixed(other._offsetFixed)
{
}

Source::Source(const std::string& name): Component(name), _wavelength(1.0),_offset(0.0),_offsetFixed(true)
{
}

Source::~Source()
{
}

Source& Source::operator=(const Source& other)
{
	if (this != &other)
	{
		Component::operator=(other);
		_wavelength = other._wavelength;
	}
	return *this;
}

Component* Source::clone() const
{
	return new Source(*this);
}

double Source::getWavelength() const
{
	return (_wavelength+_offset);
}

void Source::setWavelength(double wavelength)
{
	_wavelength=wavelength;
}

Eigen::Vector3d Source::getki() const
{
	return Eigen::Vector3d(0,1.0/(_wavelength+_offset),0.0);
}

void Source::setOffset(double off)
{
	if (!_offsetFixed)
		_offset=off;
}
void Source::setOffsetFixed(bool fixed)
{
	_offsetFixed=fixed;
}

bool Source::hasOffsetFixed() const
{
	return _offsetFixed;
}

} // end namespace Instrument

} // end namespace SX
