#include "Gonio.h"
#include "MonochromaticSource.h"

namespace SX
{

namespace Instrument
{

Source* MonochromaticSource::create(const proptree::ptree& node)
{
	return new MonochromaticSource(node);
}

MonochromaticSource::MonochromaticSource()
: Source("monochromatic source"),
  _wavelength(1.0)
{
}

MonochromaticSource::MonochromaticSource(const MonochromaticSource& other)
: Source(other),
  _wavelength(other._wavelength)
{
}

MonochromaticSource::MonochromaticSource(const std::string& name)
: Source(name),
  _wavelength(1.0)
{
}

MonochromaticSource::~MonochromaticSource()
{
}

MonochromaticSource& MonochromaticSource::operator=(const MonochromaticSource& other)
{
	if (this != &other)
	{
		Source::operator=(other);
		_wavelength=other._wavelength;
	}
	return *this;
}

MonochromaticSource::MonochromaticSource(const proptree::ptree& node) : Source(node)
{
	_wavelength=node.get<double>("wavelength");
}

Source* MonochromaticSource::clone() const
{
	return new MonochromaticSource(*this);
}

double MonochromaticSource::getWavelength() const
{
	return (_wavelength+_offset);
}

void MonochromaticSource::setWavelength(double wavelength)
{
	_wavelength=wavelength;
}

Eigen::Vector3d MonochromaticSource::getKi() const
{
	return Eigen::Vector3d(0,1.0/(_wavelength+_offset),0.0);
}

} // end namespace Instrument

} // end namespace SX
