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

#include "base/parser/EigenToVector.h"
#include "base/parser/Parser.h"
#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Component.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/raw/DataKeys.h"
#include "core/loader/RawDataReader.h"

namespace nsx {

RawDataReader::RawDataReader(const std::string& filename, Diffractometer* diffractometer)
    : IDataReader(filename, diffractometer), _parameters(), _length(0), _data()
{
    // ensure that there is at least one monochromator!
    if (_diffractometer->source().nMonochromators() == 0) {
        Monochromator mono(nsx::kw_monochromatorDefaultName);
        _diffractometer->source().addMonochromator(mono);
    }
}

void RawDataReader::addFrame(const std::string& filename)
{
    _filenames.push_back(filename);

    _nFrames = _filenames.size();

    const auto& detector_gonio = _diffractometer->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    const auto& sample_gonio = _diffractometer->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    int omega_idx = -1, phi_idx = -1, chi_idx = -1;
    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const std::string axis_name = sample_gonio.axis(i).name();
        omega_idx = axis_name == nsx::ax_omega ? int(i) : omega_idx;
        chi_idx = axis_name == nsx::ax_chi ? int(i) : chi_idx;
        phi_idx = axis_name == nsx::ax_phi ? int(i) : phi_idx;
    }

    if (omega_idx == -1 || phi_idx == -1 || chi_idx == -1)
        throw std::runtime_error("RawDataReader: could not find angle indices");

    size_t idx = _nFrames - 1;

    std::vector<double> det_states(n_detector_gonio_axes);
    std::fill(det_states.begin(), det_states.end(), 0.0);
    _detectorStates.emplace_back(std::move(det_states));

    std::vector<double> sample_states(n_sample_gonio_axes);
    std::fill(sample_states.begin(), sample_states.end(), 0.0);
    sample_states[omega_idx] = idx * _parameters.delta_omega * deg;
    sample_states[phi_idx] = idx * _parameters.delta_phi * deg;
    sample_states[chi_idx] = idx * _parameters.delta_chi * deg;
    _sampleStates.emplace_back(std::move(sample_states));
}

void RawDataReader::open() {}

void RawDataReader::close() {}

void RawDataReader::end()
{
    _metadata.add<int>(nsx::at_framesNr, int(_filenames.size()));
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

    _metadata.add<std::string>(nsx::at_diffractometer, _diffractometer->name());
    _metadata.add<double>(nsx::at_wavelength, _parameters.wavelength);
    _metadata.add<double>(nsx::at_monitorSum, 0.0);
    _metadata.add<int>(nsx::at_numor, 0.0);

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
        std::string err_msg = "LOL did not read " + filename + " successfully: ";
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
