#include "Diffractometer.h"

#include "Detector.h"
#include "DetectorFactory.h"
#include "Gonio.h"
#include "Path.h"
#include "Resources.h"
#include "Sample.h"
#include "Source.h"

namespace nsx {

sptrDiffractometer Diffractometer::build(const std::string& name)
{

    YAML::Node instrumentDefinition = findResource({"instruments",name});

    if (!instrumentDefinition["instrument"]) {
        throw std::runtime_error("Invalid instrument definition: missing 'instrument root node'");
    }

    sptrDiffractometer diffractometer;

    try {
        diffractometer = std::make_shared<Diffractometer>(Diffractometer(instrumentDefinition["instrument"]));
    } catch (std::exception& e) {
        std::string msg = "Error when reading instrument definition file: ";
        throw std::runtime_error(msg+e.what());
    }

    return diffractometer;
}

Diffractometer::Diffractometer() : _name(""), _detector(nullptr), _sample(), _source(nullptr)
{
}

Diffractometer::Diffractometer(const Diffractometer& other)
: _name(other._name),
  _detector(other._detector==nullptr ? nullptr : other._detector->clone()),
  _sample(other._sample),
  _source(other._source==nullptr ? nullptr : other._source->clone())
{
}

Diffractometer::Diffractometer(const std::string& name) : _name(name), _detector(nullptr), _sample(), _source(nullptr)
{
}

Diffractometer::Diffractometer(const YAML::Node& node)
{
    // Set the name of the diffractometer from the YAML node
    _name = node["name"].as<std::string>();

    // Build the detector from its corresponding YAML node
    _detector = sptrDetector(Detector::create(node["detector"]));

    // Build the sample from its corresponding node
    _sample = Sample(node["sample"]);

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
        _sample = other._sample;
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

void Diffractometer::setSample(const Sample &sample)
{
    _sample=sample;
}

void Diffractometer::setSource(sptrSource source)
{
    _source=source;
}

const std::string& Diffractometer::name() const
{
    return _name;
}

sptrDetector Diffractometer::detector()
{
    return _detector;
}

Sample& Diffractometer::sample()
{
    return _sample;
}

const Sample& Diffractometer::sample() const
{
    return _sample;
}

sptrSource Diffractometer::source()
{
    return _source;
}

} // end namespace nsx

