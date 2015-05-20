#include "PolychromaticSource.h"

namespace SX
{

namespace Instrument
{

Source* PolychromaticSource::create(const proptree::ptree& node)
{
	return new PolychromaticSource(node);
}

PolychromaticSource::PolychromaticSource()
: Source("polychromatic source"),
  _wavelengthMin(1.0),
  _wavelengthMax(1.0)
{
}

PolychromaticSource::PolychromaticSource(const PolychromaticSource& other)
: Source(other),
  _wavelengthMin(other._wavelengthMin),
  _wavelengthMax(other._wavelengthMax)
{
}

PolychromaticSource::PolychromaticSource(const std::string& name)
: Source(name),
  _wavelengthMin(1.0),
  _wavelengthMax(1.0)
{
}

PolychromaticSource::PolychromaticSource(const proptree::ptree& node) : Source(node)
{
	_wavelengthMin=node.get<double>("wavelength_min");
	_wavelengthMax=node.get<double>("wavelength_max");
}

PolychromaticSource::~PolychromaticSource()
{
}

PolychromaticSource& PolychromaticSource::operator=(const PolychromaticSource& other)
{
	if (this != &other)
	{
		Source::operator=(other);
		_wavelengthMin=other._wavelengthMin;
		_wavelengthMax=other._wavelengthMax;
	}
	return *this;
}

Source* PolychromaticSource::clone() const
{
	return new PolychromaticSource(*this);
}

double PolychromaticSource::getWavelength() const
{
	return (_wavelengthMin+_wavelengthMax)/2.0;
}

void PolychromaticSource::setWavelength(double wavelength)
{
	_wavelengthMin=wavelength;
	_wavelengthMax=wavelength;
}

double PolychromaticSource::getWavelengthMin() const
{
	return _wavelengthMin;
}

double PolychromaticSource::getWavelengthMax() const
{
	return _wavelengthMax;
}

void PolychromaticSource::setWavelengthMin(double wavelength)
{
	_wavelengthMin=wavelength;
}

void PolychromaticSource::setWavelengthMax(double wavelength)
{
	_wavelengthMax=wavelength;
}

Eigen::Vector3d PolychromaticSource::getKi() const
{
	double wavelength=(_wavelengthMin+_wavelengthMax)/2.0 + _offset;
	return Eigen::Vector3d(0,1.0/wavelength,0.0);
}

Eigen::Vector3d PolychromaticSource::getKiMin() const
{
	return Eigen::Vector3d(0,1.0/(_wavelengthMin+_offset),0.0);
}

Eigen::Vector3d PolychromaticSource::getKiMax() const
{
	return Eigen::Vector3d(0,1.0/(_wavelengthMax+_offset),0.0);
}

} // end namespace Instrument

} // end namespace SX
