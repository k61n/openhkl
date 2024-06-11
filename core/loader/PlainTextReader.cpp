//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/PlainTextReader.cpp
//! @brief     Implements class PlainTextReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/PlainTextReader.h"

#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"

namespace ohkl {

// NOTE: PlainTextReader needs a list of frame files which should be given later
PlainTextReader::PlainTextReader() : IDataReader("::NO-FILENAME::"), _parameters() { }

bool PlainTextReader::initRead()
{
    const bool init_success = IDataReader::initRead();
    if (!init_success)
        throw std::runtime_error("PlainTextReader::initRead(): initialisation failed");

    // NOTE: For correctness, this must be called after setting the parameters and frame files.
    // Otherwise, results in undefined behaviour.
    isInitialized = true;
    return isInitialized;
}

void PlainTextReader::addFrame(const std::string& filename)
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

void PlainTextReader::open() { }

void PlainTextReader::close() { }

const DataReaderParameters& PlainTextReader::parameters() const
{
    return _parameters;
}

void PlainTextReader::setParameters(const DataReaderParameters& parameters)
{
    _parameters = parameters;

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
}

Eigen::MatrixXi PlainTextReader::data(size_t frame)
{
    checkInit();

    const std::size_t nrows = _dataset_out->nRows();
    const std::size_t ncols = _dataset_out->nCols();

    std::string filename = _filenames.at(frame);

    std::ifstream file;
    file.open(filename, std::ios_base::binary | std::ios_base::in);
    if (!file.is_open())
        throw std::runtime_error("could not open data file " + filename);

    std::string line;

    Eigen::MatrixXd image = Eigen::MatrixXd::Zero(nrows, ncols);
    for (std::size_t row = 0; row < nrows; ++row) {
        std::getline(file, line);
        for (std::size_t col = 0; col < ncols; ++col)
            file >> image(row, col);
    }

    return image.cast<int>();
}

} // namespace ohkl
