//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5MetaDataReader.h
//! @brief     Defines class HDF5MetaDataReader (specialized templates)
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/HDF5MetaDataReader.h"

#include "base/parser/EigenToVector.h"
#include "base/utils/Logger.h"
#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/raw/DataKeys.h"

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

std::string _sampleKey(const std::string& dataset_name)
{
    return nsx::gr_DataCollections + "/" + dataset_name + "/" + nsx::gr_Sample;
}

std::string _dataKey(const std::string& dataset_name)
{
    return nsx::gr_DataCollections + "/" + dataset_name + "/" + nsx::ds_Dataset;
}

} // namespace nsx


namespace nsx {

HDF5MetaDataReader::HDF5MetaDataReader
(const std::string& filename, Diffractometer* diffractometer, std::string dataset_name)
    : IDataReader(filename, diffractometer)
    , _dataset(nullptr)
    , _space(nullptr)
    , _memspace(nullptr)
    , _blosc_filter(nullptr)
{
    H5::Group metaGroup, detectorGroup, sampleGroup;

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(filename.c_str(), H5F_ACC_RDONLY));

        // If the given dataset name is empty, find the name of the first DataCollection
        if (dataset_name.empty()) {
            H5::Group data_collections(_file->openGroup(nsx::gr_DataCollections));
            hsize_t object_num = data_collections.getNumObjs();
            if (object_num < 1) {
                throw std::runtime_error("HDF5 file '" + filename + "' has no DataCollections");
            } else {
                dataset_name = data_collections.getObjnameByIdx(0);
                // Warn about automatic selection of the first dataset when multiple datasets exist
                if (object_num >= 1) {
                    nsxlog(nsx::Level::Warning, "HDF5 file '", filename, "' has ", object_num,
                           " DataCollections; the first one, '", dataset_name, "', will be taken.");
                }
            }
        }

        nsxlog(nsx::Level::Info, "Initializing HDF5MetaDataReader to read '",
               filename, "', dataset '", dataset_name, "'");

        // TODO: make groups names compatible accross the codebase
        metaGroup = _file->openGroup("/" + _metaKey(dataset_name));
        detectorGroup = _file->openGroup("/" + _detectorKey(dataset_name));
        sampleGroup = _file->openGroup("/" + _sampleKey(dataset_name));

        // read the name of the experiment and diffractometer
        std::string experiment_name = "", diffractometer_name = "", version_str = "";

        if (_file->attrExists(nsx::at_experiment)) {
            const H5::Attribute attr =  _file->openAttribute(nsx::at_experiment);
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
        _metadata.add<std::string>(nsx::at_experiment,
                                   experiment_name.empty()? nsx::kw_experimentDefaultName : experiment_name);
        _metadata.add<std::string>(nsx::at_diffractometer,
                                   diffractometer_name.empty()? nsx::kw_diffractometerDefaultName : diffractometer_name);
        _metadata.add<std::string>(nsx::at_formatVersion,
                                   version_str);
        _metadata.add<std::string>(nsx::at_filepath, filename);
        _metadata.add<std::string>(nsx::at_datasetName, dataset_name);

    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }

    // Read the metadata group and store in metadata
    nsxlog(nsx::Level::Debug, "Reading metadata attribute of '", filename, "', dataset '", dataset_name, "'");
    const H5::StrType strVarType(H5::PredType::C_S1, H5T_VARIABLE);
    int nmeta = metaGroup.getNumAttrs();
    for (int i = 0; i < nmeta; ++i) {
        H5::Attribute attr = metaGroup.openAttribute(i);
        const std::string key { attr.getName() };
        H5::DataType typ = attr.getDataType();
        if (typ == strVarType) {
            std::string value;
            attr.read(typ, value);
            // TODO: check if this is still needed
            // override stored filename with the current one
            if (key == "filename" || key == "file_name") {
                _metadata.add<std::string>(nsx::at_datasetSources, value);
                value = filename;
            }
            _metadata.add<std::string>(key, value);
        } else if (typ == H5::PredType::NATIVE_INT32) {
            int value;
            attr.read(typ, &value);
            _metadata.add<int>(key, value);
        } else if (typ == H5::PredType::NATIVE_DOUBLE) {
            double value;
            attr.read(typ, &value);
            _metadata.add<double>(key, value);
        }
    }

    // TODO: npdone -> nr of frames
    _nFrames = _metadata.key<int>(nsx::at_frameCount);

    nsxlog(nsx::Level::Debug, "Reading detector state of '", filename, "', dataset '", dataset_name, "'");

    const auto& detector_gonio = _diffractometer->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    RowMatrixXd dm(n_detector_gonio_axes, _nFrames);
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
                if (dims[0] != _nFrames) {
                    throw std::runtime_error(
                        "Read HDF5, problem reading detector scan parameters, different "
                        "array length to number of frames");
                }
                dset.read(&dm(i, 0), H5::PredType::NATIVE_DOUBLE, space, space);
            } catch (...) {
                throw std::runtime_error("Could not read " + axis.name() + " HDF5 dataset");
            }
        } else {
            dm.row(i) = Eigen::VectorXd::Zero(_nFrames);
        }
    }

    // TODO: check units and their consistency
    // Use natural units internally (rad)
    dm *= deg;

    nsxlog(nsx::Level::Debug, "Reading gonio state of '", filename, "', dataset '", dataset_name,"'");

    _detectorStates.resize(_nFrames);

    for (unsigned int i = 0; i < _nFrames; ++i)
        _detectorStates[i] = eigenToVector(dm.col(i));

    const auto& sample_gonio = _diffractometer->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    dm.resize(n_sample_gonio_axes, _nFrames);
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
                if (dims[0] != _nFrames) {
                    throw std::runtime_error(
                        "Read HDF5, problem reading sample scan parameters, different "
                        "array length to number of frames");
                }
                dset.read(&dm(i, 0), H5::PredType::NATIVE_DOUBLE, space, space);
            } catch (...) {
                throw std::runtime_error("Coud not read " + axis.name() + " HDF5 dataset");
            }
        } else {
            dm.row(i) = Eigen::VectorXd::Zero(_nFrames);
        }
    }

    // Use natural units internally (rad)
    dm *= deg;

    _sampleStates.resize(_nFrames);
    for (unsigned int i = 0; i < _nFrames; ++i)
        _sampleStates[i] = eigenToVector(dm.col(i));

    nsxlog(nsx::Level::Info, "Finished reading the data in '", filename, "', dataset '", dataset_name, "'");
    _file->close();
}


void HDF5MetaDataReader::open()
{
    nsxlog(nsx::Level::Debug, "Opening datafile (already opened: ", _isOpened, ")");

    if (_isOpened)
        return;

    try {
        const std::string& filename = _metadata.key<std::string>(nsx::at_filepath);
        nsxlog(nsx::Level::Info, "Opening datafile '", filename, "' for read-only access");
        _file.reset(new H5::H5File(filename.c_str(), H5F_ACC_RDONLY));
    } catch (...) {
        if (_file)
            _file.reset();
        throw;
    }

    // Create new data set
    try {
        // handled automatically by HDF5 blosc filter
        _blosc_filter.reset(new HDF5BloscFilter);

        const std::string& dataset_name = _metadata.key<std::string>(nsx::at_datasetName);
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
    _nFrames = dims[0];
    _nRows = dims[1];
    _nCols = dims[2];

    nsxlog(nsx::Level::Info, "Data shape: (frames = ", _nFrames, ", rows = ", _nRows, ", columns = ", _nCols, ")");

    // Size of one hyperslab
    const hsize_t count_1frm[3] = {1, _nRows, _nCols};
    _memspace.reset(new H5::DataSpace(3, count_1frm, nullptr));
    _isOpened = true;
}


void HDF5MetaDataReader::close()
{
    if (!_isOpened)
        return;

    nsxlog(nsx::Level::Info, "Closing datafile '", _metadata.key<std::string>(nsx::at_filepath), "'");

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
