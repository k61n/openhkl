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

// TODO: The name HDF5MetaDataReader is misleading since the module does not only read metadata but the whole datastructure; something like `BaseHDF5DataReader` is better

#include "core/loader/HDF5MetaDataReader.h"

#include "base/parser/BloscFilter.h"
#include "base/parser/EigenToVector.h"
#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"

#include <iostream>
#include <memory>
#include <stdexcept>

namespace nsx {

HDF5MetaDataReader::HDF5MetaDataReader(const std::string& filename, Diffractometer* diffractometer)
    : IDataReader(filename, diffractometer), _dataset(nullptr), _space(nullptr), _memspace(nullptr)
{
    // TODO: experimentGroup -> metaGroup
    H5::Group infoGroup, experimentGroup, detectorGroup, sampleGroup;

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(filename.c_str(), H5F_ACC_RDONLY));
        // TODO: make groups names compatible accross the codebase
        infoGroup = _file->openGroup("/Info");
        experimentGroup = _file->openGroup("/Experiment");
        detectorGroup = _file->openGroup("/Data/Scan/Detector");
        sampleGroup = _file->openGroup("/Data/Scan/Sample");
    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }

    // Read the info group and store in metadata
    int ninfo = infoGroup.getNumAttrs();
    for (int i = 0; i < ninfo; ++i) {
        H5::Attribute attr = infoGroup.openAttribute(i);
        H5::DataType typ = attr.getDataType();
        std::string value;
        attr.read(typ, value);

        // TODO: check if this is still needed
        // override stored filename with the current one
        if (attr.getName() == "filename") {
            _metadata.add<std::string>("original_filename", value);
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
    _nFrames = _metadata.key<int>("npdone");

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
                        "array length to npdone");
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

    _detectorStates.resize(_nFrames);

    for (unsigned int i = 0; i < _nFrames; ++i)
        _detectorStates[i] = eigenToVector(dm.col(i));

    const auto& sample_gonio = _diffractometer->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();
    ;

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
                        "array length to npdone");
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

    _file->close();
}


void HDF5MetaDataReader::open()
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

    // handled automaticall by HDF5 blosc filter
    blosc_init();
    blosc_set_nthreads(4);

    // Register blosc filter dynamically with HDF5
    char *version, *date;
    int r = register_blosc(&version, &date);
    if (r <= 0)
        throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

    // Create new data set
    try {
        _dataset =
            std::unique_ptr<H5::DataSet>(new H5::DataSet(_file->openDataSet("/Data/Counts")));
        // Dataspace of the dataset /counts
        _space = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(_dataset->getSpace()));
    } catch (...) {
        throw;
    }
    // Gets rank of data
    const hsize_t ndims = _space->getSimpleExtentNdims();
    std::vector<hsize_t> dims(ndims), maxdims(ndims);

    // Gets dimensions of data
    _space->getSimpleExtentDims(&dims[0], &maxdims[0]);
    _nFrames = dims[0];
    _nRows = dims[1];
    _nCols = dims[2];

    // Size of one hyperslab
    hsize_t count[3];
    count[0] = 1;
    count[1] = _nRows;
    count[2] = _nCols;
    _memspace = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(3, count, nullptr));
    _isOpened = true;

    // reported by valgrind
    free(version);
    free(date);
}


void HDF5MetaDataReader::close()
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
