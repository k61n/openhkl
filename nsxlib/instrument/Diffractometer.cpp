#include "Detector.h"
#include "DetectorFactory.h"
#include "Diffractometer.h"
#include "Gonio.h"
#include "Sample.h"
#include "Source.h"

namespace nsx {

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

Diffractometer::Diffractometer(const boost::property_tree::ptree& node)
{

    std::string diffractometerName=node.get<std::string>("name");
    this->setName(diffractometerName);

    // Build the detector from its corresponding node
    const auto& detectorNode = node.get_child("detector");
    _detector = sptrDetector(Detector::create(detectorNode));

    // Build the sample from its corresponding node
    const auto& sampleNode = node.get_child("sample");
    _sample= sptrSample(Sample::create(sampleNode));

    // Build the source from its corresponding node
    const auto& sourceNode = node.get_child("source");
    _source= sptrSource(Source::create(sourceNode));
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
    */
}

Diffractometer& Diffractometer::operator=(const Diffractometer& other)
{
    if (this != &other) {
        _name = other._name;

        _detector = sptrDetector(other._detector==nullptr ? nullptr : other._detector->clone());
        _sample = sptrSample(other._detector==nullptr ? nullptr : other._sample->clone());
        _source = sptrSource(other._source==nullptr ? nullptr : other._source->clone());
    }
    return *this;
}

void Diffractometer::setDetector(sptrDetector d)
{
    _detector=d;
}

void Diffractometer::setName(const std::string& name)
{
    _name = name;
}

void Diffractometer::setSample(sptrSample sample)
{
    _sample=sample;
}
void Diffractometer::setSource(sptrSource source)
{
    _source=source;
}

const std::string& Diffractometer::getName() const
{
    return _name;
}

sptrDetector Diffractometer::getDetector()
{
    return _detector;
}

sptrSample Diffractometer::getSample()
{
    return _sample;
}

sptrSource Diffractometer::getSource()
{
    return _source;
}

std::map<unsigned int,std::string> Diffractometer::getPhysicalAxesNames() const
{
    std::map<unsigned int,std::string> names;

    if (_detector && _detector->hasGonio())	{
        auto axisIdsToNames = _detector->getGonio()->getPhysicalAxisIdToNames();
        names.insert(axisIdsToNames.begin(),axisIdsToNames.end());
    }

    if (_sample && _sample->hasGonio())	{
        auto axisIdsToNames = _sample->getGonio()->getPhysicalAxisIdToNames();
        names.insert(axisIdsToNames.begin(),axisIdsToNames.end());
    }

    if (_source && _source->hasGonio())	{
        auto axisIdsToNames = _source->getGonio()->getPhysicalAxisIdToNames();
        names.insert(axisIdsToNames.begin(),axisIdsToNames.end());
    }
    return names;
}

} // end namespace nsx

