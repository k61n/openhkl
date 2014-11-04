#include "Source.h"

namespace SX
{

namespace Instrument
{

Source::Source() : Component("source"), _wavelength(1.0)
{
}

Source::Source(const Source& other) : Component(other), _wavelength(other._wavelength)
{
}

Source::Source(const std::string& name): Component(name), _wavelength(1.0)
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
	return _wavelength;
}

void Source::setWavelength(double wavelength)
{
	_wavelength=wavelength;
}

} // end namespace Instrument

} // end namespace SX
