//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/BaseHDF5DataReader.h
//! @brief     Defines class BaseHDF5DataReader (specialized templates)
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/BaseHDF5DataReader.h"

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/mask/IMask.h"
#include "base/parser/EigenToVector.h"
#include "base/utils/Logger.h"
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/raw/DataKeys.h"
#include "core/raw/HDF5TableIO.h" // HDF5TableReader

#include <stdexcept>
#include <string>

namespace {

bool pathExists(hid_t id, const std::string& path)
{
    return H5Lexists(id, path.c_str(), H5P_DEFAULT) > 0;
}

// aux. functions to produce the group keys
std::string _metaKey(const std::string& dataset_name)
{
    return ohkl::gr_DataSets + "/" + dataset_name + "/" + ohkl::gr_Metadata;
}

std::string _detectorKey(const std::string& dataset_name)
{
    return ohkl::gr_DataSets + "/" + dataset_name + "/" + ohkl::gr_Detector;
}

std::string _sampleKey(const std::string& dataset_name)
{
    return ohkl::gr_DataSets + "/" + dataset_name + "/" + ohkl::gr_Sample;
}

std::string _dataKey(const std::string& dataset_name)
{
    return ohkl::gr_DataSets + "/" + dataset_name + "/" + ohkl::ds_Dataset;
}

std::string _maskKey(const std::string& dataset_name)
{
    return ohkl::gr_DataSets + "/" + dataset_name + "/" + ohkl::gr_Masks;
}

} // namespace


namespace ohkl {

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
            H5::Group data_collections(_file->openGroup(ohkl::gr_DataSets));
            hsize_t object_num = data_collections.getNumObjs();
            if (object_num < 1) {
                throw std::runtime_error("HDF5 file '" + _filename + "' has no DataCollections");
            } else {
                dataset_name = data_collections.getObjnameByIdx(0);
                // Warn about automatic selection of the first dataset when multiple datasets exist
                if (object_num >= 1) {
                    ohklLog(
                        ohkl::Level::Warning, "HDF5 file '", _filename, "' has ", object_num,
                        " DataCollections; the first one, '", dataset_name, "', will be taken.");
                }
            }
        }

        ohklLog(
            ohkl::Level::Info, "Initializing BaseHDF5DataReader to read '", _filename,
            "', dataset '", dataset_name, "'");

        // TODO: make groups names compatible accross the codebase
        metaGroup = _file->openGroup("/" + _metaKey(dataset_name));
        detectorGroup = _file->openGroup("/" + _detectorKey(dataset_name));
        sampleGroup = _file->openGroup("/" + _sampleKey(dataset_name));

        // read the name of the experiment and diffractometer
        std::string experiment_name, diffractometer_name, version_str;

        if (_file->attrExists(ohkl::at_experiment)) {
            const H5::Attribute attr = _file->openAttribute(ohkl::at_experiment);
            const H5::DataType attr_type = attr.getDataType();
            attr.read(attr_type, experiment_name);
        }

        if (_file->attrExists(ohkl::at_diffractometer)) {
            const H5::Attribute attr = _file->openAttribute(ohkl::at_diffractometer);
            const H5::DataType attr_type = attr.getDataType();
            attr.read(attr_type, diffractometer_name);
        }

        if (_file->attrExists(ohkl::at_formatVersion)) {
            const H5::Attribute attr = _file->openAttribute(ohkl::at_formatVersion);
            const H5::DataType attr_type = attr.getDataType();
            attr.read(attr_type, version_str);
        }

        // store the attributes in the metadata
        if (experiment_name.empty())
            throw std::runtime_error(
                "BaseHDF5DataReader::initRead: no experiment name found in file");
        _dataset_out->metadata().add<std::string>(ohkl::at_experiment, experiment_name);

        if (diffractometer_name.empty())
            throw std::runtime_error(
                "BaseHDF5DataReader::initRead: no diffractometer name found in file");
        _dataset_out->metadata().add<std::string>(ohkl::at_diffractometer, diffractometer_name);

        _dataset_out->metadata().add<std::string>(ohkl::at_formatVersion, version_str);

    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }

    // Read the metadata group and store in metadata
    ohklLog(
        ohkl::Level::Debug, "Reading metadata attribute of '", _filename, "', dataset '",
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
                _dataset_out->metadata().add<std::string>(ohkl::at_datasetSources, value);
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
    const double waveln = _dataset_out->metadata().key<double>(ohkl::at_wavelength);
    _dataset_out->diffractometer()->source().selectedMonochromator().setWavelength(waveln);

    // Compatibility 12/7/2022 - TODO: Remove the if block, as all saved experiments will have
    // baseline and gain metadata
    // Update detector baseline and gain
    if (_dataset_out->metadata().isKey(ohkl::at_baseline)
        && _dataset_out->metadata().isKey(ohkl::at_gain)) {
        const double baseline = _dataset_out->metadata().key<double>(ohkl::at_baseline);
        const double gain = _dataset_out->metadata().key<double>(ohkl::at_gain);
        _dataset_out->diffractometer()->detector()->setBaseline(baseline);
        _dataset_out->diffractometer()->detector()->setGain(gain);
    }

    const std::size_t nframes = _dataset_out->metadata().key<int>(ohkl::at_frameCount);

    ohklLog(
        ohkl::Level::Debug, "Reading detector state of '", _filename, "', dataset '", dataset_name,
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

    ohklLog(
        ohkl::Level::Debug, "Reading gonio state of '", _filename, "', dataset '", dataset_name,
        "'");

    _dataset_out->diffractometer()->setDetectorAngles(dm, nframes);

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

    _dataset_out->diffractometer()->setSampleAngles(dm, nframes);

    // Read the masks
    ohklLog(
        ohkl::Level::Debug, "Reading detector masks of '", _filename, "', data set '", dataset_name,
        "'");

    H5::Group data_group = _file->openGroup(gr_DataSets);
    if (pathExists(_file->getId(), _maskKey(dataset_name))) {
        H5::Group maskGroup = _file->openGroup("/" + _maskKey(dataset_name));
        std::size_t nmasks = _dataset_out->metadata().key<int>(ohkl::at_nMasks);
        Eigen::Matrix<int, Eigen::Dynamic, Eigen::RowMajor> maskTypes(nmasks);
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> upper(nmasks, 3);
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> lower(nmasks, 3);

        H5::DataSet mask_type_data = maskGroup.openDataSet(ohkl::ds_maskType);
        H5::DataSpace mask_type_space = mask_type_data.getSpace();
        mask_type_data.read(
            maskTypes.data(), H5::PredType::NATIVE_UINT, mask_type_space, mask_type_space);
        H5::DataSet bb_lower_data = maskGroup.openDataSet(ohkl::ds_lowerBound);
        H5::DataSpace bb_lower_space = bb_lower_data.getSpace();
        H5::DataSet bb_upper_data = maskGroup.openDataSet(ohkl::ds_upperBound);
        H5::DataSpace bb_upper_space = bb_upper_data.getSpace();
        bb_lower_data.read(
            lower.data(), H5::PredType::NATIVE_DOUBLE, bb_lower_space, bb_lower_space);
        bb_upper_data.read(
            upper.data(), H5::PredType::NATIVE_DOUBLE, bb_upper_space, bb_upper_space);

        for (std::size_t idx = 0; idx < nmasks; ++idx) {
            ohkl::MaskType mask_type = static_cast<ohkl::MaskType>(maskTypes(idx));
            Eigen::Vector3d bb_lower(lower(idx, 0), lower(idx, 1), lower(idx, 2));
            Eigen::Vector3d bb_upper(upper(idx, 0), upper(idx, 1), upper(idx, 2));
            AABB aabb(bb_lower, bb_upper);
            switch (mask_type) {
                case ohkl::MaskType::Rectangle:
                    _dataset_out->addMask(new ohkl::BoxMask(aabb));
                    break;
                case ohkl::MaskType::Ellipse:
                    _dataset_out->addMask(new ohkl::EllipseMask(aabb));
                    break;
            }
        }
    } else {
        ohklLog(
            ohkl::Level::Debug, "No masks found in '", _filename, "', data set '", dataset_name,
            "'");
    }

    _file->close();

    // Add the list of sources as metadata
    if (!_dataset_out->metadata().isKey(ohkl::at_datasetSources)) {
        _dataset_out->metadata().add<std::string>(ohkl::at_datasetSources, _filename);
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
        ohklLog(ohkl::Level::Info, "Opening datafile '", _filename, "' for read-only access");
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

        ohklLog(ohkl::Level::Debug, "Reading dataset '", dataset_name, "',");
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

    ohklLog(
        ohkl::Level::Info, "Data shape: (frames = ", nframes, ", rows = ", nrows,
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

    ohklLog(ohkl::Level::Info, "Closing datafile '", _filename, "'");

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

std::string BaseHDF5DataReader::OHKLfilepath() const
{
    return _filename;
}

} // namespace ohkl
