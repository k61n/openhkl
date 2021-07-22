//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/NexusMetaDataReader.h
//! @brief     Defines class HDF5MetaDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_NEXUSMETADATAREADER_H
#define NSX_CORE_LOADER_NEXUSMETADATAREADER_H

#include "core/raw/IDataReader.h" // inherits from

#include <H5Cpp.h>

namespace nsx {

//! IDataReader for Nexus files. Base class for NexusDataReader and FakeDataReader.

class NexusMetaDataReader : public IDataReader {
 public:

    NexusMetaDataReader(const std::string& filename, Diffractometer* diffractometer);
    bool initRead();

    NexusMetaDataReader() = delete;
    NexusMetaDataReader(const NexusMetaDataReader& other) = delete;
    ~NexusMetaDataReader() override = default;

    NexusMetaDataReader& operator=(const NexusMetaDataReader& other) = delete;

    virtual void open() override;
    virtual void close() override;


 protected:
    std::unique_ptr<H5::H5File> _file;
    std::unique_ptr<H5::DataSet> _dataset;
    std::unique_ptr<H5::DataSpace> _space;
    std::unique_ptr<H5::DataSpace> _memspace;
};

} // namespace nsx

#endif // NSX_CORE_LOADER_NEXUSMETADATAREADER_H
