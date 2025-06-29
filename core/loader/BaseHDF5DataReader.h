//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/BaseHDF5DataReader.h
//! @brief     Defines class BaseHDF5DataReader (template)
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_HDF5METADATAREADER_H
#define OHKL_CORE_LOADER_HDF5METADATAREADER_H

#include "core/loader/IDataReader.h" // inherits from
#include "core/raw/HDF5BloscFilter.h"

#include <H5Cpp.h>

// needed for method definitions

namespace ohkl {

//! IDataReader for HDF5 files. Base class for HDF5DataReader and FakeDataReader.
class BaseHDF5DataReader : public IDataReader {
 public:
    BaseHDF5DataReader() = delete;

    BaseHDF5DataReader(const BaseHDF5DataReader& other) = delete;

    BaseHDF5DataReader(const std::string& filename);

    ~BaseHDF5DataReader() = default;

    BaseHDF5DataReader& operator=(const BaseHDF5DataReader& other) = delete;

    virtual void open() override;

    virtual void close() override;

    virtual bool initRead() override;

    virtual std::string OHKLfilepath() const override;

 protected:
    std::unique_ptr<H5::H5File> _file;

    std::unique_ptr<H5::DataSet> _dataset;

    std::unique_ptr<H5::DataSpace> _space;

    std::unique_ptr<H5::DataSpace> _memspace;

    std::unique_ptr<HDF5BloscFilter> _blosc_filter;
};

} // namespace ohkl
#endif // OHKL_CORE_LOADER_HDF5METADATAREADER_H
