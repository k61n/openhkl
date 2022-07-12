//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#include "core/loader/RawDataReader.h"

#include "base/parser/EigenToVector.h"
#include "base/parser/Parser.h"
#include "base/utils/StringIO.h" // join
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/gonio/Component.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/raw/DataKeys.h"

namespace nsx {

RawDataReader::RawDataReader()
    // NOTE: RawDataReader needs a list of frame files which should be given later
    : IDataReader("::NO-FILENAME::"), _parameters(), _length(0), _data()
{
}

bool RawDataReader::initRead()
{
    const bool init_success = IDataReader::initRead();
    if (!init_success)
        throw std::runtime_error("RawDataReader::initRead(): initialisation failed");

    // NOTE: For correctness, this must be called after setting the parameters and frame files.
    // Otherwise, results in undefined behaviour.
    isInitialized = true;
    return isInitialized;
}

void RawDataReader::addFrame(const std::string& filename)
{
    checkInit();

    _filenames.push_back(filename);

    // Update sources list
    _dataset_out->metadata().add<std::string>(nsx::at_datasetSources, nsx::join(_filenames, ", "));

    const std::size_t nframes = _filenames.size();
    _dataset_out->metadata().add<int>(nsx::at_frameCount, nframes);
    _dataset_out->datashape[2] = nframes;

    const auto& detector_gonio = _dataset_out->diffractometer()->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    const auto& sample_gonio = _dataset_out->diffractometer()->sample().gonio();
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

    const std::size_t idx = nframes - 1;

    std::vector<double> det_states(n_detector_gonio_axes);
    std::fill(det_states.begin(), det_states.end(), 0.0);
    _dataset_out->diffractometer()->detectorStates.emplace_back(std::move(det_states));

    std::vector<double> sample_states(n_sample_gonio_axes);
    std::fill(sample_states.begin(), sample_states.end(), 0.0);
    sample_states[omega_idx] = idx * _parameters.delta_omega * deg;
    sample_states[phi_idx] = idx * _parameters.delta_phi * deg;
    sample_states[chi_idx] = idx * _parameters.delta_chi * deg;
    _dataset_out->diffractometer()->sampleStates.emplace_back(std::move(sample_states));
}

void RawDataReader::open() { }

void RawDataReader::close() { }

const RawDataReaderParameters& RawDataReader::parameters() const
{
    return _parameters;
}

void RawDataReader::setParameters(const RawDataReaderParameters& parameters)
{
    _parameters = parameters;

    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    _length = _parameters.bpp * nrows * ncols;
    auto& mono = _dataset_out->diffractometer()->source().selectedMonochromator();
    mono.setWavelength(_parameters.wavelength);

    _dataset_out->metadata().add<std::string>(
        nsx::at_diffractometer, _dataset_out->diffractometer()->name());
    _dataset_out->metadata().add<double>(nsx::at_wavelength, _parameters.wavelength);
    _dataset_out->metadata().add<double>(nsx::at_monitorSum, 0.0);
    _dataset_out->metadata().add<int>(nsx::at_numor, 0.0);
    _dataset_out->metadata().add<double>(nsx::at_baseline, _parameters.baseline);
    _dataset_out->metadata().add<double>(nsx::at_gain, _parameters.gain);

    _data.resize(_parameters.bpp * nrows * ncols);
}

void RawDataReader::swapEndian()
{
    if (!_parameters.swap_endian)
        return;

    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    for (std::size_t i = 0; i < nrows * ncols; ++i) {
        for (std::size_t byte = 0; byte < _parameters.bpp / 2; ++byte) {
            std::swap(
                _data[_parameters.bpp * i + byte],
                _data[_parameters.bpp * i + (_parameters.bpp - 1 - byte)]);
        }
    }
}

Eigen::MatrixXi RawDataReader::data(size_t frame)
{

    checkInit();

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
