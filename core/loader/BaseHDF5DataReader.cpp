//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/BaseHDF5DataReader.h
//! @brief     Defines class BaseHDF5DataReader (specialized templates)
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/BaseHDF5DataReader.h"

#include "base/parser/EigenToVector.h"
#include "base/utils/Logger.h"
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/raw/DataKeys.h"
#include "core/raw/HDF5TableIO.h" // HDF5TableReader

#include <stdexcept>
#include <string>

namespace {

// aux. functions to produce the group keys
std::string _metaKey(const std::string& dataset_name)
{
    return nsx::gr_DataCollections + "/" + dataset_name + "/" + nsx::gr_Metadata;
}

std::string _detectorKey(const std::string& dataset_name)
{
    return nsx::gr_DataCollections + "/" + dataset_name + "/" + nsx::gr_Detector;
}

std::string _instrumentStateKey(const std::string& dataset_name)
{
    return nsx::gr_DataCollections + "/" + dataset_name + "/" + nsx::gr_Instrument;
}

std::string _sampleKey(const std::string& dataset_name)
{
    return nsx::gr_DataCollections + "/" + dataset_name + "/" + nsx::gr_Sample;
}

std::string _dataKey(const std::string& dataset_name)
{
    return nsx::gr_DataCollections + "/" + dataset_name + "/" + nsx::ds_Dataset;
}

} // namespace


namespace nsx {

BaseHDF5DataReader::BaseHDF5DataReader(const std::string& filename)
    : IDataReader(filename)
    , _dataset(nullptr)
    , _space(nullptr)
    , _memspace(nullptr)
    , _blosc_filter(nullptr)
{
}

bool BaseHDF5DataReader::initRead()
{
    const bool init_success = IDataReader::initRead();
    if (!init_success)
        throw std::runtime_error("BaseHDF5DataReader::initRead(): initialisation failed");

    H5::Group metaGroup, detectorGroup, sampleGroup;
    std::string dataset_name = _dataset_out->name();

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(_filename.c_str(), H5F_ACC_RDONLY));

        // If the given dataset name is empty, find the name of the first DataCollection
        if (dataset_name.empty()) {
            H5::Group data_collections(_file->openGroup(nsx::gr_DataCollections));
            hsize_t object_num = data_collections.getNumObjs();
            if (object_num < 1) {
                throw std::runtime_error("HDF5 file '" + _filename + "' has no DataCollections");
            } else {
                dataset_name = data_collections.getObjnameByIdx(0);
                // Warn about automatic selection of the first dataset when multiple datasets exist
                if (object_num >= 1) {
                    nsxlog(
                        nsx::Level::Warning, "HDF5 file '", _filename, "' has ", object_num,
                        " DataCollections; the first one, '", dataset_name, "', will be taken.");
                }
            }
        }

        nsxlog(
            nsx::Level::Info, "Initializing BaseHDF5DataReader to read '", _filename,
            "', dataset '", dataset_name, "'");

        // TODO: make groups names compatible accross the codebase
        metaGroup = _file->openGroup("/" + _metaKey(dataset_name));
        detectorGroup = _file->openGroup("/" + _detectorKey(dataset_name));
        sampleGroup = _file->openGroup("/" + _sampleKey(dataset_name));

        // read the name of the experiment and diffractometer
        std::string experiment_name, diffractometer_name, version_str;

        if (_file->attrExists(nsx::at_experiment)) {
            const H5::Attribute attr = _file->openAttribute(nsx::at_experiment);
            const H5::DataType attr_type = attr.getDataType();
            attr.read(attr_type, experiment_name);
        }

        if (_file->attrExists(nsx::at_diffractometer)) {
            const H5::Attribute attr = _file->openAttribute(nsx::at_diffractometer);
            const H5::DataType attr_type = attr.getDataType();
            attr.read(attr_type, diffractometer_name);
        }

        if (_file->attrExists(nsx::at_formatVersion)) {
            const H5::Attribute attr = _file->openAttribute(nsx::at_formatVersion);
            const H5::DataType attr_type = attr.getDataType();
            attr.read(attr_type, version_str);
        }

        // store the attributes in the metadata
        _dataset_out->metadata().add<std::string>(
            nsx::at_experiment,
            experiment_name.empty() ? nsx::kw_experimentDefaultName : experiment_name);
        _dataset_out->metadata().add<std::string>(
            nsx::at_diffractometer,
            diffractometer_name.empty() ? nsx::kw_diffractometerDefaultName : diffractometer_name);
        _dataset_out->metadata().add<std::string>(nsx::at_formatVersion, version_str);

    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }

    // Read the metadata group and store in metadata
    nsxlog(
        nsx::Level::Debug, "Reading metadata attribute of '", _filename, "', dataset '",
        dataset_name, "'");
    const H5::StrType strVarType(H5::PredType::C_S1, H5T_VARIABLE);
    int nmeta = metaGroup.getNumAttrs();
    for (int i = 0; i < nmeta; ++i) {
        H5::Attribute attr = metaGroup.openAttribute(i);
        const std::string key{attr.getName()};
        H5::DataType typ = attr.getDataType();
        if (typ == strVarType) {
            std::string value;
            attr.read(typ, value);
            // TODO: check if this is still needed
            // override stored filename with the current one
            if (key == "filename" || key == "file_name") {
                _dataset_out->metadata().add<std::string>(nsx::at_datasetSources, value);
                value = _filename;
            }
            _dataset_out->metadata().add<std::string>(key, value);
        } else if (typ == H5::PredType::NATIVE_INT32) {
            int value;
            attr.read(typ, &value);
            _dataset_out->metadata().add<int>(key, value);
        } else if (typ == H5::PredType::NATIVE_DOUBLE) {
            double value;
            attr.read(typ, &value);
            _dataset_out->metadata().add<double>(key, value);
        }
    }

    // Update the monochromator wavelength
    const double waveln = _dataset_out->metadata().key<double>(nsx::at_wavelength);
    _dataset_out->diffractometer()->source().selectedMonochromator().setWavelength(waveln);

    const std::size_t nframes = _dataset_out->metadata().key<int>(nsx::at_frameCount);

    nsxlog(
        nsx::Level::Debug, "Reading detector state of '", _filename, "', dataset '", dataset_name,
        "'");

    const auto& detector_gonio = _dataset_out->diffractometer()->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    RowMatrixXd dm(n_detector_gonio_axes, nframes);
    for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
        const auto& axis = detector_gonio.axis(i);
        if (axis.physical()) {
            try {
                H5::DataSet dset = detectorGroup.openDataSet(axis.name());
                H5::DataSpace space(dset.getSpace());
                hsize_t dim = space.getSimpleExtentNdims();
                if (dim != 1) {
                    throw std::runtime_error(
                        "Read HDF5, problem reading detector scan parameters, dimension "
                        "of array should be 1");
                }
                std::vector<hsize_t> dims(dim), maxdims(dim);
                space.getSimpleExtentDims(&dims[0], &maxdims[0]);
                if (dims[0] != nframes) {
                    throw std::runtime_error(
                        "Read HDF5, problem reading detector scan parameters, different "
                        "array length to number of frames");
                }
                dset.read(&dm(i, 0), H5::PredType::NATIVE_DOUBLE, space, space);
            } catch (...) {
                throw std::runtime_error("Could not read " + axis.name() + " HDF5 dataset");
            }
        } else {
            dm.row(i) = Eigen::VectorXd::Zero(nframes);
        }
    }

    // TODO: check units and their consistency
    // Use natural units internally (rad)
    dm *= deg;

    nsxlog(
        nsx::Level::Debug, "Reading gonio state of '", _filename, "', dataset '", dataset_name,
        "'");

    _dataset_out->diffractometer()->detectorStates.resize(nframes);

    for (unsigned int i = 0; i < nframes; ++i)
        _dataset_out->diffractometer()->detectorStates[i] = eigenToVector(dm.col(i));

    const auto& sample_gonio = _dataset_out->diffractometer()->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    dm.resize(n_sample_gonio_axes, nframes);
    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const auto& axis = sample_gonio.axis(i);
        if (axis.physical()) {
            try {
                H5::DataSet dset = sampleGroup.openDataSet(axis.name());
                H5::DataSpace space(dset.getSpace());
                hsize_t dim = space.getSimpleExtentNdims();
                if (dim != 1) {
                    throw std::runtime_error(
                        "Read HDF5, problem reading sample scan parameters, dimension of "
                        "array should be 1");
                }
                std::vector<hsize_t> dims(dim), maxdims(dim);
                space.getSimpleExtentDims(&dims[0], &maxdims[0]);
                if (dims[0] != nframes) {
                    throw std::runtime_error(
                        "Read HDF5, problem reading sample scan parameters, different "
                        "array length to number of frames");
                }
                dset.read(&dm(i, 0), H5::PredType::NATIVE_DOUBLE, space, space);
            } catch (...) {
                throw std::runtime_error("Coud not read " + axis.name() + " HDF5 dataset");
            }
        } else {
            dm.row(i) = Eigen::VectorXd::Zero(nframes);
        }
    }

    // Use natural units internally (rad)
    dm *= deg;

    _dataset_out->diffractometer()->sampleStates.resize(nframes);
    for (unsigned int i = 0; i < nframes; ++i)
        _dataset_out->diffractometer()->sampleStates[i] = eigenToVector(dm.col(i));

    _file->close();

    // Add the list of sources as metadata
    if (!_dataset_out->metadata().isKey(nsx::at_datasetSources)) {
        _dataset_out->metadata().add<std::string>(nsx::at_datasetSources, _filename);
    }

    isInitialized = true;
    return isInitialized;
}


void BaseHDF5DataReader::open()
{
    checkInit();

    if (_isOpened)
        return;

    try {
        nsxlog(nsx::Level::Info, "Opening datafile '", _filename, "' for read-only access");
        _file.reset();
        _file.reset(new H5::H5File(_filename.c_str(), H5F_ACC_RDONLY));
    } catch (...) {
        if (_file)
            _file.reset();
        throw;
    }

    const std::string& dataset_name = _dataset_out->name();
    // Create new data set
    try {
        // handled automatically by HDF5 blosc filter
        _blosc_filter.reset();
        _blosc_filter.reset(new HDF5BloscFilter);

        nsxlog(nsx::Level::Debug, "Reading dataset '", dataset_name, "',");
        _dataset.reset(new H5::DataSet(_file->openDataSet("/" + _dataKey(dataset_name))));
        // Dataspace of the dataset /counts
        _space.reset(new H5::DataSpace(_dataset->getSpace()));
    } catch (...) {
        throw;
    }
    // Gets rank of data
    const hsize_t ndims = _space->getSimpleExtentNdims();
    std::vector<hsize_t> dims(ndims), maxdims(ndims);

    // Gets dimensions of data
    _space->getSimpleExtentDims(dims.data(), maxdims.data());
    const std::size_t nframes = dims[0], nrows = dims[1], ncols = dims[2];
    _dataset_out->datashape[0] = ncols;
    _dataset_out->datashape[1] = nrows;
    _dataset_out->datashape[2] = nframes;

    nsxlog(
        nsx::Level::Info, "Data shape: (frames = ", nframes, ", rows = ", nrows,
        ", columns = ", ncols, ")");

    // Size of one hyperslab
    const hsize_t count_1frm[3] = {1, nrows, ncols};
    _memspace.reset(new H5::DataSpace(3, count_1frm, nullptr));
    _isOpened = true;
}


void BaseHDF5DataReader::close()
{
    if (!_isOpened)
        return;

    nsxlog(nsx::Level::Info, "Closing datafile '", _filename, "'");

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

std::string BaseHDF5DataReader::NSXfilepath() const
{
    return _filename;
}

} // namespace nsx
