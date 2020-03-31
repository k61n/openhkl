//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5MetaDataReader.cpp
//! @brief     Implements class HDF5MetaDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/NexusMetaDataReader.h"

#include "base/parser/BloscFilter.h"
#include "base/parser/EigenToVector.h"
#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"

#include <memory>
#include <stdexcept>

namespace nsx {


NexusMetaDataReader::NexusMetaDataReader(
    const std::string& filename, Diffractometer* diffractometer)
    : IDataReader(filename, diffractometer), _dataset(nullptr), _space(nullptr), _memspace(nullptr)
{
    try {
        // std::cout << "frames = " << _nFrames << ", rows = " << _nRows << ", cols = " << _nCols <<
        // std::endl; std::cout << "nMonos = " << _diffractometer->source().nMonochromators() <<
        // std::endl;

        _file = std::unique_ptr<H5::H5File>(new H5::H5File(filename.c_str(), H5F_ACC_RDONLY));

        H5::Group entryGroup = _file->openGroup("/entry0");
        H5::Group instrumentGroup = entryGroup.openGroup("instrument");
        H5::Group dataGroup = entryGroup.openGroup("data_scan");
        H5::Group monitorGroup = entryGroup.openGroup("monitor");

        // get numor
        int numor = -1;
        entryGroup.openDataSet("run_number").read(&numor, H5::PredType::NATIVE_INT);

        // get number of frames
        dataGroup.openDataSet("total_steps").read(&_nFrames, H5::PredType::NATIVE_INT);

        // get wavelength
        double wavelength = -1.;
        entryGroup.openDataSet("wavelength").read(&wavelength, H5::PredType::NATIVE_DOUBLE);
        auto& mono = _diffractometer->source().selectedMonochromator();
        mono.setWavelength(wavelength);

        // get monitor
        double monitor = -1.;
        monitorGroup.openDataSet("monsum").read(&monitor, H5::PredType::NATIVE_DOUBLE);
        std::cout << monitor << std::endl;

        // check instrument name
        char instr_name[128];
        instrumentGroup.openDataSet("name").read(instr_name, H5::StrType(0, sizeof(instr_name)));
        if (_diffractometer->name() != instr_name)
            throw std::runtime_error("Nexus: instrument name mismatch");

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
        _metadata.add<std::string>("Instrument", _diffractometer->name());
        _metadata.add<double>("wavelength", wavelength);
        _metadata.add<double>("monitor", monitor);
        _metadata.add<int>("Numor", numor);
        _metadata.add<int>("npdone", _nFrames);
        _metadata.add<std::string>("title", title);
        _metadata.add<std::string>("experiment_identifier", experiment_id);
        _metadata.add<std::string>("start_time", start_time);
        _metadata.add<std::string>("end_time", end_time);
        _metadata.add<double>("time", time);


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
        if (dimScannedVars != 2)
            throw std::runtime_error("Nexus: invalid dimension of scanned variable block");

        hsize_t dimsScannedVars[2];
        spaceScannedVars.getSimpleExtentDims(&dimsScannedVars[0]);
        if (dimsScannedVars[0] != 5 || dimsScannedVars[1] != _nFrames)
            throw std::runtime_error("Nexus: invalid dimension of scanned variable block");

        double* scanned_vars = new double[dimsScannedVars[0] * dimsScannedVars[1]];
        dsScannedVars.read(scanned_vars, H5::PredType::NATIVE_DOUBLE);


        // get scan parameters for the sample
        const auto& sample_gonio = _diffractometer->sample().gonio();
        size_t n_sample_gonio_axes = sample_gonio.nAxes();

        // TODO: match indices with "variables_names" in nexus file
        int omega_idx = -1, phi_idx = -1, chi_idx = -1;
        for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
            const std::string axis_name = sample_gonio.axis(i).name();
            omega_idx = axis_name == "omega" ? int(i) : omega_idx;
            chi_idx = axis_name == "chi" ? int(i) : chi_idx;
            phi_idx = axis_name == "phi" ? int(i) : phi_idx;
        }

        if (omega_idx == -1 || phi_idx == -1 || chi_idx == -1)
            throw std::runtime_error("Nexus: could not find angle indices");


        // iterate instrument configuration for all frames
        for (size_t frame = 0; frame < _nFrames; ++frame) {
            const auto& detector_gonio = _diffractometer->detector()->gonio();
            size_t n_detector_gonio_axes = detector_gonio.nAxes();
            std::vector<double> det_states(n_detector_gonio_axes);
            _detectorStates.emplace_back(det_states);

            std::vector<double> sample_states(n_sample_gonio_axes);
            sample_states[omega_idx] = scanned_vars[_nFrames * omega_idx + frame] * deg;
            sample_states[phi_idx] = scanned_vars[_nFrames * phi_idx + frame] * deg;
            sample_states[chi_idx] = scanned_vars[_nFrames * chi_idx + frame] * deg;
            _sampleStates.emplace_back(sample_states);
        }


        delete[] scanned_vars;
        _file->close();
    } catch (H5::Exception& e) {
        std::string what = "Nexus: " + e.getDetailMsg();
        throw std::runtime_error(what);
    }
}


void NexusMetaDataReader::open()
{
    if (_isOpened)
        return;

    try {
        _file = std::unique_ptr<H5::H5File>(
            new H5::H5File(_metadata.key<std::string>("filename").c_str(), H5F_ACC_RDONLY));
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
    _nFrames = dims[0];
    _nRows = dims[2];
    _nCols = dims[1];
    // std::cout << "frames = " << _nFrames << ", rows = " << _nRows << ", cols = " << _nCols <<
    // std::endl;


    // Size of one hyperslab
    hsize_t count[3];
    count[0] = 1;
    count[1] = _nRows;
    count[2] = _nCols;
    _memspace = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(3, count, nullptr));
    _isOpened = true;
}


void NexusMetaDataReader::close()
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

} // namespace nsx
