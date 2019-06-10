//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5MetaDataReader.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_HDF5METADATAREADER_H
#define CORE_LOADER_HDF5METADATAREADER_H


//@ #include <string>

#include "H5Cpp.h"
#include "core/loader/IDataReader.h"

namespace nsx {

//! \brief Read the experiment metadata from and HDF file.
class HDF5MetaDataReader : public IDataReader {

public:
    HDF5MetaDataReader() = delete;

    HDF5MetaDataReader(const HDF5MetaDataReader& other) = delete;

    HDF5MetaDataReader(const std::string& filename, Diffractometer* instrument);

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

#endif // CORE_LOADER_HDF5METADATAREADER_H
