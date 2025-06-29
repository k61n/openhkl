//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/BaseNexusDataReader.cpp
//! @brief     Implements class BaseNexusDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/BaseNexusDataReader.h"

#include "base/parser/BloscFilter.h"
#include "base/parser/EigenToVector.h"
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/raw/DataKeys.h"

#include <iostream>
#include <memory>
#include <stdexcept>

namespace ohkl {

BaseNexusDataReader::BaseNexusDataReader(const std::string& filename)
    : IDataReader(filename), _dataset(nullptr), _space(nullptr), _memspace(nullptr)
{
}

bool BaseNexusDataReader::initRead()
{
    const bool init_success = IDataReader::initRead();
    if (!init_success)
        throw std::runtime_error("BaseNexusDataReader::initRead(): initialisation failed");

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(_filename.c_str(), H5F_ACC_RDONLY));

        H5::Group rootGroup = _file->openGroup("/");
        H5::Group entryGroup = rootGroup.openGroup("/entry0");
        H5::Group instrumentGroup = entryGroup.openGroup("instrument");
        H5::Group dataGroup = entryGroup.openGroup("data_scan");
        H5::Group monitorGroup = entryGroup.openGroup("monitor");

        // get numor
        int numor = -1;
        entryGroup.openDataSet("run_number").read(&numor, H5::PredType::NATIVE_INT);

        // get number of intended and actual frames
        std::size_t totalSteps = 0;
        dataGroup.openDataSet("total_steps").read(&totalSteps, H5::PredType::NATIVE_INT);
        int nFrames_i32;
        dataGroup.openDataSet("actual_step").read(&nFrames_i32, H5::PredType::NATIVE_INT);
        // TODO: Choose a ui32 type for storing nFrames to avoid this cast
        const std::size_t nFrames{static_cast<std::size_t>(nFrames_i32)};

        // get wavelength
        double wavelength = -1.;
        entryGroup.openDataSet("wavelength").read(&wavelength, H5::PredType::NATIVE_DOUBLE);
        auto& mono = _dataset_out->diffractometer()->source().selectedMonochromator();
        mono.setWavelength(wavelength);

        // get monitor
        double monitor = -1.;
        monitorGroup.openDataSet("monsum").read(&monitor, H5::PredType::NATIVE_DOUBLE);

        // check instrument name
        char instr_name[128];
        instrumentGroup.openDataSet("name").read(instr_name, H5::StrType(0, sizeof(instr_name)));
        if (_dataset_out->diffractometer()->name() != instr_name)
            throw std::runtime_error("Nexus: instrument name mismatch");

        // get initial instrument positions
        double omega = 0;
        double chi = 0;
        double phi = 0;
        instrumentGroup.openDataSet("omega/value").read(&omega, H5::PredType::NATIVE_DOUBLE);
        instrumentGroup.openDataSet("chi/value").read(&chi, H5::PredType::NATIVE_DOUBLE);
        instrumentGroup.openDataSet("phi/value").read(&phi, H5::PredType::NATIVE_DOUBLE);


        // non-essential metadata fields
        char start_time[128], end_time[128];
        entryGroup.openDataSet("start_time").read(start_time, H5::StrType(0, sizeof(start_time)));
        entryGroup.openDataSet("end_time").read(end_time, H5::StrType(0, sizeof(end_time)));

        char title[128], experiment_id[128];
        entryGroup.openDataSet("title").read(title, H5::StrType(0, sizeof(title)));
        entryGroup.openDataSet("experiment_identifier")
            .read(experiment_id, H5::StrType(0, sizeof(experiment_id)));

        double time;
        entryGroup.openDataSet("time").read(&time, H5::PredType::NATIVE_DOUBLE);


        // set metadata
        _dataset_out->metadata().add<std::string>(
            ohkl::at_diffractometer, _dataset_out->diffractometer()->name());
        _dataset_out->metadata().add<double>(ohkl::at_wavelength, wavelength);
        _dataset_out->metadata().add<double>(ohkl::at_monitorSum, monitor);
        _dataset_out->metadata().add<int>(ohkl::at_numor, numor);
        _dataset_out->metadata().add<int>(ohkl::at_frameCount, nFrames_i32);
        _dataset_out->metadata().add<int>(ohkl::at_totalSteps, totalSteps);
        _dataset_out->metadata().add<std::string>(ohkl::at_title, title);
        _dataset_out->metadata().add<std::string>(ohkl::at_experiment, experiment_id);
        _dataset_out->metadata().add<std::string>(ohkl::at_startTime, start_time);
        _dataset_out->metadata().add<std::string>(ohkl::at_endTime, end_time);
        _dataset_out->metadata().add<double>(ohkl::at_time, time);

        // put root attributes into meta data
        for (int i = 0; i < rootGroup.getNumAttrs(); ++i) {
            H5::Attribute attr = rootGroup.openAttribute(i);
            H5::DataType typ = attr.getDataType();
            std::string value;
            attr.read(typ, value);

            // override stored filename with the current one
            if (attr.getName() == "file_name" || attr.getName() == "filename")
                _dataset_out->metadata().add<std::string>(ohkl::at_datasetSources, value);
            else
                _dataset_out->metadata().add<std::string>(attr.getName(), value);
        }


        // Add the list of sources as metadata
        if (!_dataset_out->metadata().isKey(ohkl::at_datasetSources)) {
            _dataset_out->metadata().add<std::string>(ohkl::at_datasetSources, _filename);
        }

        // which axis is scanned?
        int scanned_axes[4];
        dataGroup.openDataSet("scanned_variables/variables_names/scanned")
            .read(&scanned_axes, H5::PredType::NATIVE_INT);
        /*std::cout << "scanned axes: ";
        for(int i=0; i<4; ++i)
            std::cout << scanned_axes[i] << ", ";
        std::cout << std::endl;*/


        // get scanned variables
        H5::DataSet dsScannedVars = dataGroup.openDataSet("scanned_variables/data");
        H5::DataSpace spaceScannedVars(dsScannedVars.getSpace());
        hsize_t dimScannedVars = spaceScannedVars.getSimpleExtentNdims();
        // std::cout << "dimScannedVars: " << dimScannedVars << std::endl;
        if (dimScannedVars != 2)
            throw std::runtime_error("Nexus: invalid dimension of scanned variable block");

        hsize_t dimsScannedVars[2] = {0, 0};
        spaceScannedVars.getSimpleExtentDims(&dimsScannedVars[0]);
        // std::cout << "dimsScannedVars[0]: " << dimsScannedVars[0] << std::endl;
        // std::cout << "dimsScannedVars[1]: " << dimsScannedVars[1] << std::endl;
        if (dimsScannedVars[0] != 5)
            throw std::runtime_error("Nexus: invalid dimension of scanned variable block");
        if (dimsScannedVars[1] != nFrames)
            throw std::runtime_error(
                "Nexus: mismatch between scanned variable block and number of frames");

        double* scanned_vars = new double[dimsScannedVars[0] * dimsScannedVars[1]];
        dsScannedVars.read(scanned_vars, H5::PredType::NATIVE_DOUBLE);


        // get scan parameters for the sample
        const auto& sample_gonio = _dataset_out->diffractometer()->sample().gonio();
        size_t n_sample_gonio_axes = sample_gonio.nAxes();

        // TODO: match indices with "variables_names" in nexus file
        int omega_idx = -1, phi_idx = -1, chi_idx = -1;
        for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
            const std::string axis_name = sample_gonio.axis(i).name();
            omega_idx = axis_name == ohkl::ax_omega ? int(i) : omega_idx;
            chi_idx = axis_name == ohkl::ax_chi ? int(i) : chi_idx;
            phi_idx = axis_name == ohkl::ax_phi ? int(i) : phi_idx;
        }

        if (omega_idx == -1 || phi_idx == -1 || chi_idx == -1)
            throw std::runtime_error("Nexus: could not find angle indices");


        // iterate instrument configuration for all frames
        for (size_t frame = 0; frame < nFrames; ++frame) {
            const auto& detector_gonio = _dataset_out->diffractometer()->detector()->gonio();
            size_t n_detector_gonio_axes = detector_gonio.nAxes();
            std::vector<double> det_states(n_detector_gonio_axes);
            _dataset_out->diffractometer()->addDetectorAngles(det_states);

            std::vector<double> sample_states(n_sample_gonio_axes);
            std::fill(sample_states.begin(), sample_states.end(), 0.);

            // initial positions
            sample_states[omega_idx] = omega * deg;
            sample_states[chi_idx] = chi * deg;
            sample_states[phi_idx] = phi * deg;

            // only read the values for which the scanned_axes flag is set
            if (scanned_axes[omega_idx])
                sample_states[omega_idx] = scanned_vars[nFrames * omega_idx + frame] * deg;
            if (scanned_axes[chi_idx])
                sample_states[chi_idx] = scanned_vars[nFrames * chi_idx + frame] * deg;
            if (scanned_axes[phi_idx])
                sample_states[phi_idx] = scanned_vars[nFrames * phi_idx + frame] * deg;

            // std::cout << sample_states[omega_idx] << " " << sample_states[chi_idx] << " " <<
            // sample_states[phi_idx] << std::endl;
            _dataset_out->diffractometer()->addSampleAngles(sample_states);
        }


        delete[] scanned_vars;
        _file->close();
    } catch (H5::Exception& e) {
        std::string what = "Nexus: " + e.getDetailMsg();
        throw std::runtime_error(what);
    }

    isInitialized = true;
    return isInitialized;
}


void BaseNexusDataReader::open()
{
    checkInit();

    if (_isOpened)
        return;

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(_filename.c_str(), H5F_ACC_RDONLY));
    } catch (...) {
        if (_file)
            _file.reset();
        throw;
    }


    // handled automatically by HDF5 blosc filter
    blosc_init();
    blosc_set_nthreads(4);

    char *version, *date;
    int r = register_blosc(&version, &date);
    if (r <= 0)
        throw std::runtime_error("Problem registering BLOSC filter in HDF5 Nexus library");
    free(version);
    free(date);


    // create new data set
    _dataset = std::unique_ptr<H5::DataSet>(
        new H5::DataSet(_file->openDataSet("/entry0/data_scan/detector_data/data")));

    // dataspace of the dataset /counts
    _space = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(_dataset->getSpace()));


    // Gets rank of data
    const hsize_t ndims = _space->getSimpleExtentNdims();
    std::vector<hsize_t> dims(ndims), maxdims(ndims);

    // Gets dimensions of data
    _space->getSimpleExtentDims(&dims[0], &maxdims[0]);
    const std::size_t nframes = dims[0], nrows = dims[2], ncols = dims[1];
    _dataset_out->datashape[0] = ncols;
    _dataset_out->datashape[1] = nrows;
    _dataset_out->datashape[2] = nframes;

    // Size of one hyperslab
    hsize_t count[3];
    count[0] = 1;
    count[1] = nrows;
    count[2] = ncols;
    _memspace = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(3, count, nullptr));
    _isOpened = true;
}


void BaseNexusDataReader::close()
{
    if (!_isOpened)
        return;

    _file->close();
    _space->close();
    _memspace->close();
    _dataset->close();
    _space.reset();
    _memspace.reset();
    _dataset.reset();
    _file.reset();
    _isOpened = false;
}

} // namespace ohkl
