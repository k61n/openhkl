#include "Diffractometer.h"

#include "Detector.h"
#include "DetectorFactory.h"
#include "Gonio.h"
#include "Path.h"
#include "Sample.h"
#include "Source.h"

namespace nsx {

sptrDiffractometer Diffractometer::build(const std::string& name)
{

    std::string diffractometerFile = buildPath({"instruments",name+".yml"},applicationDataPath());

    YAML::Node instrumentDefinition;

    try {
        instrumentDefinition = YAML::LoadFile(diffractometerFile);
    }
    catch (const std::exception& error) {
        throw std::runtime_error("Error when opening instrument definition file");
    }

    if (!instrumentDefinition["instrument"]) {
        throw std::runtime_error("Invalid instrument definition: missing 'instrument root node'");
    }

    sptrDiffractometer diffractometer;

    try {
        diffractometer = std::make_shared<Diffractometer>(Diffractometer(instrumentDefinition["instrument"]));
    }
    catch (...)
    {
        throw std::runtime_error("Error when reading instrument definition file");
    }

    return diffractometer;
}

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

Diffractometer::Diffractometer(const YAML::Node& node)
{
    // Set the name of the diffractometer from the YAML node
    _name = node["name"].as<std::string>();

    // Build the detector from its corresponding YAML node
    _detector = sptrDetector(Detector::create(node["detector"]));

    // Build the sample from its corresponding node
    _sample= sptrSample(Sample::create(node["sample"]));

    // Build the source from its corresponding node
    _source= sptrSource(Source::create(node["source"]));
}

Diffractometer::~Diffractometer()
{
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

