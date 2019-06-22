//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/RawDataReader.cpp
//! @brief     Implements class RawDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "core/detector/Detector.h"
#include "core/gonio/Component.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "base/parser/EigenToVector.h"
#include "base/parser/Parser.h"
#include "core/loader/RawDataReader.h"
#include "base/utils/Units.h"

namespace nsx {

RawDataReader::RawDataReader(const std::string& filename, Diffractometer* diffractometer)
    : IDataReader(filename, diffractometer), _parameters(), _length(0), _data()
{
    // ensure that there is at least one monochromator!
    if (_diffractometer->source().nMonochromators() == 0) {
        Monochromator mono("mono");
        _diffractometer->source().addMonochromator(mono);
    }

    setParameters(_parameters);

    addFrame(filename);
}

void RawDataReader::addFrame(const std::string& filename)
{
    _filenames.push_back(filename);

    _nFrames = _filenames.size();

    const auto& detector_gonio = _diffractometer->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();
    Eigen::VectorXd dm(n_detector_gonio_axes);
    _detectorStates.push_back(eigenToVector(dm));

    // Getting Scan parameters for the sample
    const auto& sample_gonio = _diffractometer->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    dm.resize(n_sample_gonio_axes);

    int omega = -1;
    int phi = -1;
    int chi = -1;
    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const std::string axis_name = sample_gonio.axis(i).name();
        omega = axis_name == "omega" ? int(i) : omega;
        chi = axis_name == "chi" ? int(i) : chi;
        phi = axis_name == "phi" ? int(i) : phi;
    }

    assert(omega != -1);
    assert(phi != -1);
    assert(chi != -1);

    size_t idx = _nFrames - 1;

    dm(omega) = idx * _parameters.delta_omega;
    dm(phi) = idx * _parameters.delta_phi;
    dm(chi) = idx * _parameters.delta_chi;

    // Use natural units internally (rad)
    dm *= deg;

    _sampleStates.push_back(eigenToVector(dm));
}

void RawDataReader::open() {}

void RawDataReader::close() {}

void RawDataReader::end()
{
    _metadata.add<int>("npdone", int(_filenames.size()));
}

const RawDataReaderParameters& RawDataReader::parameters() const
{
    return _parameters;
}

void RawDataReader::setParameters(const RawDataReaderParameters& parameters)
{
    _parameters = parameters;

    _length = _parameters.bpp * _nRows * _nCols;
    auto& mono = _diffractometer->source().selectedMonochromator();
    mono.setWavelength(_parameters.wavelength);

    _metadata.add<std::string>("Instrument", _diffractometer->name());
    _metadata.add<double>("wavelength", _parameters.wavelength);
    _metadata.add<double>("monitor", 0.0);
    _metadata.add<int>("Numor", 0.0);

    _data.resize(_parameters.bpp * _nRows * _nCols);
}

void RawDataReader::swapEndian()
{
    if (!_parameters.swap_endian)
        return;

    for (unsigned int i = 0; i < _nRows * _nCols; ++i) {
        for (unsigned int byte = 0; byte < _parameters.bpp / 2; ++byte) {
            std::swap(
                _data[_parameters.bpp * i + byte],
                _data[_parameters.bpp * i + (_parameters.bpp - 1 - byte)]);
        }
    }
}

Eigen::MatrixXi RawDataReader::data(size_t frame)
{
    std::string filename = _filenames.at(frame);

    std::ifstream file;
    file.open(filename, std::ios_base::binary | std::ios_base::in);

    if (!file.is_open())
        throw std::runtime_error("could not open data file " + filename);

    file.seekg(0, std::ios_base::end);

    if (_length != size_t(file.tellg())) {
        std::string err_msg = "data file " + filename + " is not of the expected size: ";
        err_msg += "expected " + std::to_string(_length) + " bytes but found "
            + std::to_string(file.tellg());
        throw std::runtime_error(err_msg);
    }

    file.seekg(0, std::ios_base::beg);
    file.read(&_data[0], long(_length));

    if (_length != size_t(file.gcount())) {
        std::string err_msg = "did not read " + filename + " successfully: ";
        err_msg += "expected " + std::to_string(_length) + " bytes but read "
            + std::to_string(file.gcount());
        throw std::runtime_error(err_msg);
    }

    swapEndian();

    switch (_parameters.bpp) {
    case 1: return matrixFromData<uint8_t>().cast<int>();
    case 2: return matrixFromData<uint16_t>().cast<int>();
    case 3: return matrixFromData<uint32_t>().cast<int>();
    default: throw std::runtime_error("bpp unsupported: " + std::to_string(_parameters.bpp));
    }
}

} // namespace nsx
