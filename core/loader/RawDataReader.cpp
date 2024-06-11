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

#include "core/loader/RawDataReader.h"

#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"

#include <filesystem>

namespace ohkl {

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

    const std::size_t idx = nframes - 1;

    _dataset_out->diffractometer()->addSampleAngles(idx, _parameters);
    _dataset_out->diffractometer()->addDetectorAngles(_parameters);
}

void RawDataReader::open() { }

void RawDataReader::close() { }

const DataReaderParameters& RawDataReader::parameters() const
{
    return _parameters;
}

void RawDataReader::setParameters(const DataReaderParameters& parameters)
{
    _parameters = parameters;

    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    _length = _parameters.bytes_per_pixel * nrows * ncols;
    auto& mono = _dataset_out->diffractometer()->source().selectedMonochromator();
    mono.setWavelength(_parameters.wavelength);

    _dataset_out->metadata().add<std::string>(
        ohkl::at_diffractometer, _dataset_out->diffractometer()->name());
    _dataset_out->metadata().add<double>(ohkl::at_wavelength, _parameters.wavelength);
    _dataset_out->metadata().add<double>(ohkl::at_monitorSum, 0.0);
    _dataset_out->metadata().add<int>(ohkl::at_numor, 0);
    _dataset_out->metadata().add<double>(ohkl::at_baseline, _parameters.baseline);
    _dataset_out->metadata().add<double>(ohkl::at_gain, _parameters.gain);
    switch (_parameters.bytes_per_pixel) {
        case 2: {
            _dataset_out->metadata().add<int>(ohkl::at_bitDepth, 16);
            break;
        }
        case 4: {
            _dataset_out->metadata().add<int>(ohkl::at_bitDepth, 32);
            break;
        }
        default:
            throw std::runtime_error(
                "bytes_per_pixel unsupported: " + std::to_string(_parameters.bytes_per_pixel));
    }

    _data.resize(_parameters.bytes_per_pixel * nrows * ncols);
}

void RawDataReader::swapEndian()
{
    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    for (std::size_t i = 0; i < nrows * ncols; ++i) {
        for (std::size_t byte = 0; byte < _parameters.bytes_per_pixel / 2; ++byte) {
            std::swap(
                _data[_parameters.bytes_per_pixel * i + byte],
                _data[_parameters.bytes_per_pixel * i + (_parameters.bytes_per_pixel - 1 - byte)]);
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

    switch (_parameters.bytes_per_pixel) {
        case 1: return matrixFromData<uint8_t>().cast<int>();
        case 2: return matrixFromData<uint16_t>().cast<int>();
        case 4: return matrixFromData<uint32_t>().cast<int>();
        default:
            throw std::runtime_error(
                "bytes_per_pixel unsupported: " + std::to_string(_parameters.bytes_per_pixel));
    }
}

} // namespace ohkl
