//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/PckDataReader.h
//! @brief     Defines class PckDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "PckDataReader.h"

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
#include "3rdparty/ccp4/pack_c.h"

namespace ohkl {

PckDataReader::PckDataReader() : IDataReader("::NO-FILENAME::") {}

bool PckDataReader::initRead()
{
    const bool init_success = IDataReader::initRead();
    if (!init_success)
        throw std::runtime_error("PckDataReader::initRead(): initialisation failed");

    // see RawDataReader.cpp with that following comment.
    // NOTE: For correctness, this must be called after setting the parameters and frame files.
    // Otherwise, results in undefined behaviour.
    isInitialized = true;
    return isInitialized;
}

// for compatability with the rest of ohkl needed
void PckDataReader::addFrame(const std::string& filename)
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
        throw std::runtime_error("PckDataReader::addFrame: could not find angle indices");

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

Eigen::MatrixXi PckDataReader::data(size_t frame)
{
    checkInit();

    // // Check if we can process this stream
    // if (_tiff_meta_data.compression != 1) // no compressed tiff files
    //     throw std::runtime_error("PckDataReader::data: Compressed Tiff files are not supported");
    // if (_tiff_meta_data.planar_config != PLANARCONFIG_CONTIG) // need planar based tiff files
    //     throw std::runtime_error(
    //         "PckDataReader::data: Only PLANARCONFIG_CONTIG tiff files are supported");
    // if (_tiff_meta_data.photometric > 1) // tiff files can only have gray colors
    //     throw std::runtime_error("PckDataReader::data: Colored Tiff files are not supported");

    // // Read the tiff using OpenCV
    Eigen::MatrixXi eigen_image, resized;
    // cv::Mat image = cv::imread(_filenames.at(frame), cv::IMREAD_UNCHANGED);

    // if (_parameters.rebin_size != 1) {
    //     if (_tiff_meta_data.image_width % _parameters.rebin_size != 0
    //         || _tiff_meta_data.image_length % _parameters.rebin_size != 0)
    //         throw std::runtime_error(
    //             "PckDataReader::data: image columns/rows not divisible by rebin size");
    //     cv::Mat resized_image;
    //     // Resize the image using box filter (area interpolation), i.e. map a n x n pixel
    //     // grid to a single pixel with the mean value
    //     int target_cols = _tiff_meta_data.image_width / _parameters.rebin_size;
    //     int target_rows = _tiff_meta_data.image_length / _parameters.rebin_size;
    //     cv::resize(image, resized_image, cv::Size(target_cols, target_rows), cv::INTER_AREA);
    //     cv::cv2eigen(resized_image, eigen_image);
    //     return eigen_image;
    // }

    // cv::cv2eigen(image, eigen_image);
    return eigen_image;
}


void PckDataReader::setParameters(const DataReaderParameters& parameters)
{
    _parameters = parameters;
    _parameters.log(Level::Info);

    auto& mono = _dataset_out->diffractometer()->source().selectedMonochromator();
    mono.setWavelength(_parameters.wavelength);

    _dataset_out->metadata().add<std::string>(
        ohkl::at_diffractometer, _dataset_out->diffractometer()->name());
    _dataset_out->metadata().add<double>(ohkl::at_wavelength, _parameters.wavelength);
    _dataset_out->metadata().add<double>(ohkl::at_monitorSum, 0.0);
    _dataset_out->metadata().add<int>(ohkl::at_numor, 0.0);
    _dataset_out->metadata().add<double>(ohkl::at_baseline, _parameters.baseline);
    _dataset_out->metadata().add<double>(ohkl::at_gain, _parameters.gain);
}

} // namespace ohkl
