//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5MetaDataReader.h
//! @brief     Defines class HDF5MetaDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_HDF5METADATAREADER_H
#define NSX_CORE_LOADER_HDF5METADATAREADER_H

#include "core/raw/IDataReader.h" // inherits from

#include <H5Cpp.h>

namespace nsx {

//! IDataReader for HDF5 files. Base class for HDF5DataReader and FakeDataReader.

class HDF5MetaDataReader : public IDataReader {
 public:
    HDF5MetaDataReader() = delete;

    HDF5MetaDataReader(const HDF5MetaDataReader& other) = delete;

    HDF5MetaDataReader(const std::string& filename, Diffractometer* diffractometer);

    ~HDF5MetaDataReader() override = default;

    HDF5MetaDataReader& operator=(const HDF5MetaDataReader& other) = delete;

    virtual void open() override;

    virtual void close() override;

 protected:
    std::unique_ptr<H5::H5File> _file;

    std::unique_ptr<H5::DataSet> _dataset;

    std::unique_ptr<H5::DataSpace> _space;

    std::unique_ptr<H5::DataSpace> _memspace;
};

} // namespace nsx

#endif // NSX_CORE_LOADER_HDF5METADATAREADER_H
