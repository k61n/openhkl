//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Diffractometer.cpp
//! @brief     Implements class Diffractometer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/Diffractometer.h"

#include "base/utils/Path.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorFactory.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/HardwareParameters.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/raw/DataKeys.h"

namespace nsx {

Diffractometer* Diffractometer::create(const std::string& name)
{
    YAML::Node instrumentDefinition = findResource(name);

    Diffractometer* diffractometer;
    try {
        diffractometer = new Diffractometer(instrumentDefinition[nsx::ym_instrument]);
    } catch (std::exception& e) {
        std::string msg = "Error when reading instrument definition file: ";
        throw std::runtime_error(msg + e.what());
    }

    return diffractometer;
}

Diffractometer::Diffractometer() : _detector(nullptr), _sample(), _source() { }

Diffractometer::Diffractometer(const YAML::Node& node)
{
    // Sets the name of the diffractometer from the YAML node
    _name = node[nsx::ym_instrumentName].as<std::string>();

    // Build the detector from its corresponding YAML node
    _detector.reset(Detector::create(node[nsx::ym_detector]));

    // Build the sample from its corresponding node
    _sample = Sample(node[nsx::ym_sample]);

    // Build the source from its corresponding node
    _source = Source(node[nsx::ym_source]);
}

Diffractometer::~Diffractometer() = default;

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

InstrumentState Diffractometer::instrumentState(const std::size_t frame_idx) const
{
    assert(frame_idx < sampleStates.size());
    assert(frame_idx < detectorStates.size());

    InstrumentState state(const_cast<Diffractometer*>(this));  // TODO: Find a better way instead of const_cast

    // compute transformations
    const auto& detector_gonio = _detector->gonio();
    const auto& sample_gonio = _sample.gonio();

    Eigen::Transform<double, 3, Eigen::Affine> detector_trans =
        detector_gonio.affineMatrix(detectorStates[frame_idx]);
    Eigen::Transform<double, 3, Eigen::Affine> sample_trans =
        sample_gonio.affineMatrix(sampleStates[frame_idx]);

    state.detectorOrientation = detector_trans.rotation();
    state.sampleOrientation = Eigen::Quaterniond(sample_trans.rotation());

    state.detectorPositionOffset = detector_trans.translation();
    state.samplePosition = sample_trans.translation();

    state.ni = _source.selectedMonochromator().ki().rowVector();
    state.ni.normalize();
    state.wavelength = _source.selectedMonochromator().wavelength();

    return state;
}

} // namespace nsx
