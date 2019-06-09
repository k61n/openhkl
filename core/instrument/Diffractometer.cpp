//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Diffractometer.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/Diffractometer.h"

#include "Resources.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorFactory.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/utils/Path.h"

namespace nsx {

Diffractometer* Diffractometer::create(const std::string& name)
{

    YAML::Node instrumentDefinition = findResource({"instruments", name});

    if (!instrumentDefinition["instrument"]) {
        throw std::runtime_error("Invalid instrument definition: missing 'instrument root node'");
    }

    Diffractometer* diffractometer;

    try {
        diffractometer = new Diffractometer(instrumentDefinition["instrument"]);
    } catch (std::exception& e) {
        std::string msg = "Error when reading instrument definition file: ";
        throw std::runtime_error(msg + e.what());
    }

    return diffractometer;
}

Diffractometer::Diffractometer() : _name(""), _detector(nullptr), _sample(), _source() {}

Diffractometer::Diffractometer(const Diffractometer& other)
    : _name(other._name)
    , _detector(other._detector ? other._detector->clone() : nullptr)
    , _sample(other._sample)
    , _source(other._source)
{
}

Diffractometer* Diffractometer::clone() const
{
    return new Diffractometer(*this);
}

Diffractometer::Diffractometer(const std::string& name)
    : _name(name), _detector(nullptr), _sample(), _source()
{
}

Diffractometer::Diffractometer(const YAML::Node& node)
{
    // Sets the name of the diffractometer from the YAML node
    _name = node["name"].as<std::string>();

    // Build the detector from its corresponding YAML node
    _detector.reset(Detector::create(node["detector"]));

    // Build the sample from its corresponding node
    _sample = Sample(node["sample"]);

    // Build the source from its corresponding node
    _source = Source(node["source"]);
}

Diffractometer::~Diffractometer() {}

Diffractometer& Diffractometer::operator=(const Diffractometer& other)
{
    if (this != &other) {
        _name = other._name;
        _detector.reset(other._detector ? other._detector->clone() : nullptr);
        _sample = other._sample;
        _source = other._source;
    }
    return *this;
}

void Diffractometer::setDetector(std::unique_ptr<Detector> detector)
{
    _detector = std::move(detector);
}

void Diffractometer::setName(const std::string& name)
{
    _name = name;
}

const std::string& Diffractometer::name() const
{
    return _name;
}

Detector* Diffractometer::detector()
{
    return _detector.get();
}

const Detector* Diffractometer::detector() const
{
    return _detector.get();
}

Sample& Diffractometer::sample()
{
    return _sample;
}

const Sample& Diffractometer::sample() const
{
    return _sample;
}

void Diffractometer::setSample(const Sample& sample)
{
    _sample = sample;
}

Source& Diffractometer::source()
{
    return _source;
}

const Source& Diffractometer::source() const
{
    return _source;
}

void Diffractometer::setSource(const Source& source)
{
    _source = source;
}

} // end namespace nsx
