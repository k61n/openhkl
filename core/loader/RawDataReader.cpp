//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/RawDataReader.cpp
//! @brief     Implements class RawDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <cstring>
#include <filesystem>
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

namespace ohkl {

void DataReaderParameters::LoadDataFromFile(std::string file)
{
    dataset_name = fileBasename(file);

    std::size_t pos1 = file.find_last_of("/");
    std::size_t pos0 = (file.substr(0, pos1 - 1)).find_last_of("/");
    std::size_t pos2 = file.find_last_of(".");

    if (pos1 == std::string::npos || pos0 == std::string::npos || pos2 == std::string::npos)
        return;

    std::string dir = "data_" + file.substr(pos0 + 1, pos1 - pos0 - 1);
    std::string readme = file.substr(0, pos1 + 1) + dir + ".readme";

    std::ifstream fin(readme.c_str(), std::ios::in);

    if (fin.is_open() || fin.good()) {
        fin.seekg(0, std::ios::end);
        auto fsize = fin.tellg();
        fin.seekg(0, std::ios::beg);

        if (fsize > 0) {
            std::string buffer;
            buffer.resize(fsize);
            fin.read(buffer.data(), fsize);
            fin.close();

            std::remove_if(buffer.begin(), buffer.end(), isspace);

            auto omega_pos = buffer.find("Omegarange:");
            if (omega_pos != std::string::npos) {
                auto nl_pos = buffer.find(";", omega_pos);
                std::string a = buffer.substr(omega_pos + 11, nl_pos - 1);
                delta_omega = std::stod(a);
            }

            auto lambda_pos = buffer.find("Lambda:");
            if (lambda_pos != std::string::npos) {
                auto nl_pos = buffer.find(";", lambda_pos);
                std::string b = buffer.substr(lambda_pos + 7, nl_pos - 1);
                wavelength = std::stod(b);
            }
        }
    }
}

void DataReaderParameters::log(const Level& level) const
{
    ohklLog(level, "DataReaderParameters::log:");
    ohklLog(level, "wavelength     = ", wavelength);
    ohklLog(level, "delta_omega    = ", delta_omega);
    ohklLog(level, "delta_chi      = ", delta_chi);
    ohklLog(level, "delta_phi      = ", delta_phi);
    ohklLog(level, "swap_endian    = ", swap_endian);
    ohklLog(level, "baseline       = ", baseline);
    ohklLog(level, "gain           = ", gain);
}

void RawDataReaderParameters::log(const Level& level) const
{
    DataReaderParameters::log(level);
    ohklLog(level, "RawDataReaderParameters::log:");
    ohklLog(level, "row_major      = ", row_major);
    ohklLog(level, "bpp            = ", bpp);
}

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
    _dataset_out->metadata().add<std::string>(
        ohkl::at_datasetSources, ohkl::join(_filenames, ", "));

    const std::size_t nframes = _filenames.size();
    _dataset_out->metadata().add<int>(ohkl::at_frameCount, nframes);
    _dataset_out->datashape[2] = nframes;

    const auto& detector_gonio = _dataset_out->diffractometer()->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    const auto& sample_gonio = _dataset_out->diffractometer()->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    int omega_idx = -1, phi_idx = -1, chi_idx = -1;
    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const std::string axis_name = sample_gonio.axis(i).name();
        omega_idx = axis_name == ohkl::ax_omega ? int(i) : omega_idx;
        chi_idx = axis_name == ohkl::ax_chi ? int(i) : chi_idx;
        phi_idx = axis_name == ohkl::ax_phi ? int(i) : phi_idx;
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
        ohkl::at_diffractometer, _dataset_out->diffractometer()->name());
    _dataset_out->metadata().add<double>(ohkl::at_wavelength, _parameters.wavelength);
    _dataset_out->metadata().add<double>(ohkl::at_monitorSum, 0.0);
    _dataset_out->metadata().add<int>(ohkl::at_numor, 0);
    _dataset_out->metadata().add<double>(ohkl::at_baseline, _parameters.baseline);
    _dataset_out->metadata().add<double>(ohkl::at_gain, _parameters.gain);
    switch (_parameters.bpp) {
        case 1: {
            _dataset_out->metadata().add<int>(ohkl::at_bitDepth, 8);
            break;
        }
        case 2: {
            _dataset_out->metadata().add<int>(ohkl::at_bitDepth, 16);
            break;
        }
        case 3: {
            _dataset_out->metadata().add<int>(ohkl::at_bitDepth, 32);
            break;
        }
        default: throw std::runtime_error("bpp unsupported: " + std::to_string(_parameters.bpp));
    }

    _data.resize(_parameters.bpp * nrows * ncols);
}

void RawDataReader::swapEndian()
{
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
    const size_t fsize = std::filesystem::file_size(filename);
    if (fsize != _length) {
        std::string err_msg = "data file " + filename + " is not of the expected size: "
            + "expected " + std::to_string(_length) + " bytes but found " + std::to_string(fsize);
        throw std::runtime_error(err_msg);
    }

    std::ifstream file;
    file.open(filename, std::ios_base::binary | std::ios_base::in);
    if (!file.is_open())
        throw std::runtime_error("could not open data file " + filename);

    file.read(&_data[0], long(_length));
    if (size_t(file.gcount()) != _length) {
        std::string err_msg = "cannot load file " + filename + ": " + "expected "
            + std::to_string(_length) + " bytes but found " + std::to_string(file.gcount());
        throw std::runtime_error(err_msg);
    }

    if (_parameters.swap_endian)
        swapEndian();

    switch (_parameters.bpp) {
        case 1: return matrixFromData<uint8_t>().cast<int>();
        case 2: return matrixFromData<uint16_t>().cast<int>();
        case 4: return matrixFromData<uint32_t>().cast<int>();
        default: throw std::runtime_error("bpp unsupported: " + std::to_string(_parameters.bpp));
    }
}

} // namespace ohkl
