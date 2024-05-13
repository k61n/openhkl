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
PlainTextReader::PlainTextReader()
    : IDataReader("::NO-FILENAME::"), _parameters()
{
}

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
        throw std::runtime_error("PlainTextReader: could not find angle indices");

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
        for (std::size_t col =0; col < ncols; ++col)
            file >> image(row, col);
    }

    return image.cast<int>();
}

} // namespace ohkl
