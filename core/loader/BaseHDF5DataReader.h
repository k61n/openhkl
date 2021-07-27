//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/BaseHDF5DataReader.h
//! @brief     Defines class BaseHDF5DataReader (template)
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_HDF5METADATAREADER_H
#define NSX_CORE_LOADER_HDF5METADATAREADER_H

#include "core/raw/DataKeys.h" // kw_datasetDefaultName
#include "core/raw/HDF5BloscFilter.h"
#include "core/loader/IDataReader.h" // inherits from

#include <H5Cpp.h>
#include <memory>
#include <string>

// needed for method definitions

namespace nsx {

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

 protected:
    std::unique_ptr<H5::H5File> _file;

    std::unique_ptr<H5::DataSet> _dataset;

    std::unique_ptr<H5::DataSpace> _space;

    std::unique_ptr<H5::DataSpace> _memspace;

    std::unique_ptr<HDF5BloscFilter> _blosc_filter;
};

} // namespace nsx
#endif // NSX_CORE_LOADER_HDF5METADATAREADER_H
