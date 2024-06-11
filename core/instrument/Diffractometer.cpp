//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Diffractometer.cpp
//! @brief     Implements class Diffractometer
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/Diffractometer.h"

#include "InstrumentParameters.h"
#include "base/parser/EigenToVector.h"
#include "base/utils/Path.h"
#include "base/utils/Units.h"
#include "core/gonio/Axis.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorFactory.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/InstrumentState.h"
#include "core/loader/IDataReader.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

Diffractometer* Diffractometer::create(const std::string& name)
{
    YAML::Node instrumentDefinition = Instrument::findResource(name);

    Diffractometer* diffractometer;
    diffractometer = new Diffractometer(instrumentDefinition[ohkl::ym_instrument]);

    return diffractometer;
}

Diffractometer::Diffractometer(const YAML::Node& node)
    : _sample(node[ohkl::ym_sample])
    , _source(node[ohkl::ym_source])
{
    // Sets the name of the diffractometer from the YAML node
    _name = node[ohkl::ym_instrumentName].as<std::string>();

    // Build the detector from its corresponding YAML node
    _detector.reset(Detector::create(node[ohkl::ym_detector]));
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

void Diffractometer::addSampleAngles(std::size_t frame_idx, const DataReaderParameters& params)
{
    auto axis_names = _sample.gonio().axisNames();

    std::vector<double> sample_angles;
    for (const auto& name : axis_names) {
        double angle = 0;
        if (name == ax_omega)
            angle = params.delta_omega;
        else if (name == ax_chi)
            angle = params.delta_chi;
        else if (name == ax_phi)
            angle = params.delta_phi;
        else
            throw std::runtime_error(
                "Diffractometer::addSampleState: unrecognised goniometer axis name");
        sample_angles.emplace_back(angle * frame_idx * deg);
    }

    _sample_angles.emplace_back(std::move(sample_angles));
}

void Diffractometer::addSampleAngles(const std::vector<double>& angles)
{
    _sample_angles.emplace_back(angles);
}

void Diffractometer::addDetectorAngles(const DataReaderParameters& params)
{
    auto axis_names = _detector->gonio().axisNames();

    std::vector<double> det_angles;
    for (const auto& name : axis_names) {
        double angle = 0;
        if (name == ax_2thetaGamma)
            angle = params.twotheta_gamma;
        else if (name == ax_2thetaNu)
            angle = params.twotheta_nu;
        else
            throw std::runtime_error(
                "Diffractometer::addDetectorState: unrecognised goniometer axis name");
        det_angles.emplace_back(angle * deg);
    }
    _detector_angles.emplace_back(std::move(det_angles));
}

void Diffractometer::addDetectorAngles(const std::vector<double>& angles)
{
    _detector_angles.emplace_back(angles);
}

void Diffractometer::setSampleAngles(const RowMatrixXd& mat, std::size_t nframes)
{
    _sample_angles.resize(nframes);
    for (std::size_t idx = 0; idx < nframes; ++idx)
        _sample_angles[idx] = eigenToVector(mat.col(idx));
}

void Diffractometer::setDetectorAngles(const RowMatrixXd& mat, std::size_t nframes)
{
    _detector_angles.resize(nframes);
    for (std::size_t idx = 0; idx < nframes; ++idx)
        _detector_angles[idx] = eigenToVector(mat.col(idx));
}

InstrumentState Diffractometer::instrumentState(const std::size_t frame_idx)
{
    return InstrumentState::state(this, frame_idx);
}

} // namespace ohkl
