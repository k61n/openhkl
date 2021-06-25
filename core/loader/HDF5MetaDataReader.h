//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5MetaDataReader.h
//! @brief     Defines class HDF5MetaDataReader (template)
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_HDF5METADATAREADER_H
#define NSX_CORE_LOADER_HDF5METADATAREADER_H

#include "core/raw/HDF5BloscFilter.h"
#include "core/raw/IDataReader.h" // inherits from
#include "core/raw/DataKeys.h"

#include <H5Cpp.h>
#include <string>

// needed for method definitions
#include "base/parser/EigenToVector.h"
#include "base/utils/Logger.h"
#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"

#include <memory>
#include <stdexcept>

namespace nsx {


// TODO: Check if the legacy reader is still needed
//! Determines the type of the HDF5-Reader;
//! legacy reader for backward-compatibility (should be removed after standardization)
enum HDF5ReaderType { LegacyReader = -1, ExperimentReader = 0 };

//! IDataReader for HDF5 files. Base class for HDF5DataReader and FakeDataReader.
template <HDF5ReaderType ReaderT>
class HDF5MetaDataReader : public IDataReader {
 public:
    const HDF5ReaderType reader_type = ReaderT;

    HDF5MetaDataReader() = delete;

    HDF5MetaDataReader(const HDF5MetaDataReader& other) = delete;

    HDF5MetaDataReader(const std::string& filename, Diffractometer* diffractometer,
                       const std::string& group_name = "");

    ~HDF5MetaDataReader() = default;

    HDF5MetaDataReader& operator=(const HDF5MetaDataReader& other) = delete;

    virtual void open() override;

    virtual void close() override;

 protected:
    std::unique_ptr<H5::H5File> _file;

    std::unique_ptr<H5::DataSet> _dataset;

    std::unique_ptr<H5::DataSpace> _space;

    std::unique_ptr<H5::DataSpace> _memspace;

    std::unique_ptr<HDF5BloscFilter> _blosc_filter;

    virtual std::string _metaKey(const std::string& group_name) const;
    virtual std::string _infoKey(const std::string& group_name) const;
    virtual std::string _detectorKey(const std::string& group_name) const;
    virtual std::string _sampleKey(const std::string& group_name) const;
    virtual std::string _dataKey(const std::string& group_name) const;
};

//-----------------------------------------------------------------------------80


template <HDF5ReaderType ReaderT>
HDF5MetaDataReader<ReaderT>::HDF5MetaDataReader(
    const std::string& filename, Diffractometer* diffractometer, const std::string& group_name)
    : IDataReader(filename, diffractometer)
    , _dataset(nullptr)
    , _space(nullptr)
    , _memspace(nullptr)
    , _blosc_filter(nullptr)
{
    nsxlog(nsx::Level::Debug, "Initializing HDF5MetaDataReader<", ReaderT, ">",
           "to read ", filename, ", group ", group_name);

    // TODO: experimentGroup -> metaGroup
    H5::Group infoGroup, experimentGroup, detectorGroup, sampleGroup;

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(filename.c_str(), H5F_ACC_RDONLY));

        // TODO: make groups names compatible accross the codebase
        infoGroup = _file->openGroup("/" + _infoKey(group_name));
        experimentGroup = _file->openGroup("/" + _metaKey(group_name)); // = metaGroup
        detectorGroup = _file->openGroup("/" + _detectorKey(group_name));
        sampleGroup = _file->openGroup("/" + _sampleKey(group_name));

        // TODO: Check consistency of the metadata
        _metadata.add<std::string>(nsx::at_filepath, filename);
        _metadata.add<std::string>(nsx::at_datasetName, group_name);

    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }

    // Read the info group and store in metadata
    nsxlog(nsx::Level::Debug, "Reading metadata attribute of", filename, ", group ", group_name);
    int ninfo = infoGroup.getNumAttrs();
    for (int i = 0; i < ninfo; ++i) {
        H5::Attribute attr = infoGroup.openAttribute(i);
        H5::DataType typ = attr.getDataType();
        std::string value;
        attr.read(typ, value);

	// Do not overwrite main attributes which are stored before
	if (attr.getName() == "real_path" || attr.getName() == "group_name")
	    continue;

        // TODO: check if this is still needed
        // override stored filename with the current one
        if (attr.getName() == "filename" || attr.getName() == "file_name") {
            _metadata.add<std::string>(nsx::at_datasetSources, value);
            value = filename;
        }
        _metadata.add<std::string>(attr.getName(), value);
    }

    // Read the experiment group and store all int and double attributes in
    // metadata
    int nexps = experimentGroup.getNumAttrs();
    for (int i = 0; i < nexps; ++i) {
        H5::Attribute attr = experimentGroup.openAttribute(i);
        H5::DataType typ = attr.getDataType();
        if (typ == H5::PredType::NATIVE_INT32) {
            int value;
            attr.read(typ, &value);
            _metadata.add<int>(attr.getName(), value);
        }
        if (typ == H5::PredType::NATIVE_DOUBLE) {
            double value;
            attr.read(typ, &value);
            _metadata.add<double>(attr.getName(), value);
        }
    }

    // TODO: npdone -> nr of frames
    _nFrames = _metadata.key<int>(nsx::at_framesNr);

    nsxlog(nsx::Level::Debug, "Reading detector state of", filename, ", group ", group_name);

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

    nsxlog(nsx::Level::Debug, "Reading gonio state of", filename, ", group ", group_name);

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

    nsxlog(nsx::Level::Debug, "Finished reading the data in", filename, ", group ", group_name);
    _file->close();
}

template <HDF5ReaderType ReaderT>
void HDF5MetaDataReader<ReaderT>::open()
{
    nsxlog(nsx::Level::Debug, "Opening datafile (already opened: ", _isOpened, ")");

    if (_isOpened)
        return;

    try {
        const std::string& filename = _metadata.key<std::string>(nsx::at_filepath);
        nsxlog(nsx::Level::Info, "Opening datafile ", filename, "for read-only access");
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

        const std::string& group_name = _metadata.key<std::string>(nsx::at_datasetName);
        nsxlog(nsx::Level::Debug, "Reading data group", group_name);
        _dataset.reset(new H5::DataSet(_file->openDataSet("/" + _dataKey(group_name))));
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

    nsxlog(nsx::Level::Info, "Data shape: (frames =", _nFrames, ", rows =", _nRows, ", columns = ", _nCols, ")");

    // Size of one hyperslab
    const hsize_t count_1frm[3] = {1, _nRows, _nCols};
    _memspace.reset(new H5::DataSpace(3, count_1frm, nullptr));
    _isOpened = true;
}

template <HDF5ReaderType ReaderT>
void HDF5MetaDataReader<ReaderT>::close()
{
    if (!_isOpened)
        return;

    nsxlog(nsx::Level::Info, "Closing datafile", _metadata.key<std::string>(nsx::at_filepath));

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
#endif // NSX_CORE_LOADER_HDF5METADATAREADER_H
