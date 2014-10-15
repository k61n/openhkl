#include "Diffractometer.h"
#include "Detector.h"
#include "Sample.h"
#include "Source.h"

namespace SX
{

namespace Instrument
{

Diffractometer::Diffractometer() : _name(""), _detector(nullptr), _sample(nullptr), _source(nullptr)
{
}

Diffractometer::Diffractometer(const Diffractometer& other)
: _name(other._name),
  _detector(other._detector==nullptr ? nullptr : other._detector->clone()),
  _sample(new Sample(*other._sample)),
  _source(new Source(*other._source))
{
}

Diffractometer::Diffractometer(const std::string& name) : _name(name), _detector(nullptr), _sample(nullptr), _source(nullptr)
{
}

Diffractometer::~Diffractometer()
{
	if (_detector)
		delete _detector;
	if (_sample)
		delete _sample;
	if (_source)
		delete _source;
}

Diffractometer& Diffractometer::operator=(const Diffractometer& other)
{
	if (this != &other)
	{
		_name = other._name;
		_detector = other._detector==nullptr ? nullptr : other._detector->clone();
		_sample = other._detector==nullptr ? nullptr : new Sample(*other._sample);
		_source = other._source==nullptr ? nullptr : new Source(*other._source);
	}
	return *this;
}

void Diffractometer::setDetector(Detector* d)
{
	_detector=d;
}

void Diffractometer::setSample(Sample* s)
{
	_sample=s;
}
void Diffractometer::setSource(Source* s)
{
	_source=s;
}

Detector* Diffractometer::getDetector()
{
	return _detector;
}

Sample* Diffractometer::getSample()
{
	return _sample;
}

Source* Diffractometer::getSource()
{
	return _source;
}




}
} /* namespace SX */
