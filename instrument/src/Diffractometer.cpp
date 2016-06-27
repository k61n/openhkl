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
  _sample(other._sample==nullptr ? nullptr : other._sample->clone()),
  _source(other._source==nullptr ? nullptr : other._source->clone())
{
}

Diffractometer::Diffractometer(const std::string& name) : _name(name), _detector(nullptr), _sample(nullptr), _source(nullptr)
{
}

Diffractometer::Diffractometer(const proptree::ptree& node)
{

	std::string diffractometerName=node.get<std::string>("name");
	this->setName(diffractometerName);

	// Build the detector from its corresponding node
	const property_tree::ptree& detectorNode = node.get_child("detector");
    _detector = std::shared_ptr<Detector>(Detector::create(detectorNode));

	// Build the sample from its corresponding node
	const property_tree::ptree& sampleNode = node.get_child("sample");
    _sample= std::shared_ptr<Sample>(Sample::create(sampleNode));

	// Build the source from its corresponding node
	const property_tree::ptree& sourceNode = node.get_child("source");
    _source= std::shared_ptr<Source>(Source::create(sourceNode));
}

Diffractometer::~Diffractometer()
{
    // no longer necessary to explicitly delete since we switched to smart pointers
    /*
	if (_detector)
		delete _detector;
	if (_sample)
		delete _sample;
	if (_source)
		delete _source;
    /**/
}

Diffractometer& Diffractometer::operator=(const Diffractometer& other)
{
	if (this != &other)
	{
		_name = other._name;

        _detector = std::shared_ptr<Detector>(other._detector==nullptr ? nullptr : other._detector->clone());
        _sample = std::shared_ptr<Sample>(other._detector==nullptr ? nullptr : other._sample->clone());
        _source = std::shared_ptr<Source>(other._source==nullptr ? nullptr : other._source->clone());
	}
	return *this;
}

void Diffractometer::setDetector(std::shared_ptr<Detector> d)
{
	_detector=d;
}

void Diffractometer::setName(const std::string& name)
{
	_name = name;
}

void Diffractometer::setSample(std::shared_ptr<Sample> s)
{
	_sample=s;
}
void Diffractometer::setSource(std::shared_ptr<Source> s)
{
	_source=s;
}

const std::string& Diffractometer::getName() const
{
	return _name;
}

std::shared_ptr<Detector> Diffractometer::getDetector()
{
	return _detector;
}

std::shared_ptr<Sample> Diffractometer::getSample()
{
	return _sample;
}

std::shared_ptr<Source> Diffractometer::getSource()
{
	return _source;
}

}
} /* namespace SX */
