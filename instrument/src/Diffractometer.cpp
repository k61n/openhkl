#include "Detector.h"
#include "DetectorFactory.h"
#include "Diffractometer.h"
#include "Gonio.h"
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

Diffractometer::Diffractometer(const ptree& node)
{

	std::string diffractometerName=node.get<std::string>("name");
	this->setName(diffractometerName);

	// Build the detector

	// Get the detector node
	const property_tree::ptree& detectorNode = node.get_child("detector");

	// Get the detector type
	std::string detectorType=detectorNode.get<std::string>("<xmlattr>.type");

	// Get the detector name
	std::string detectorName=detectorNode.get<std::string>("name");

	// Create an instance of the detector factory
	DetectorFactory* detectorFactory=DetectorFactory::Instance();
	// Fetch the detector from the factory
	_detector = detectorFactory->create(detectorType,detectorName);
	_detector->buildFromXML(detectorNode);

	// Build the sample

	// Get the sample node
	const property_tree::ptree& sampleNode = node.get_child("sample");
    _sample= new Sample();
	_sample->buildFromXML(sampleNode);

	// Build the source

	// Get the source node
	const property_tree::ptree& sourceNode = node.get_child("source");
    _source= new Source();
	_source->buildFromXML(sourceNode);
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

void Diffractometer::setName(const std::string& name)
{
	_name = name;
}

void Diffractometer::setSample(Sample* s)
{
	_sample=s;
}
void Diffractometer::setSource(Source* s)
{
	_source=s;
}

const std::string& Diffractometer::getName() const
{
	return _name;
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
