//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/BaseNexusDataReader.h
//! @brief     Defines class BaseNexusDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_NEXUSMETADATAREADER_H
#define OHKL_CORE_LOADER_NEXUSMETADATAREADER_H

#include "core/loader/IDataReader.h" // inherits from

#include <H5Cpp.h>

#include <memory>

namespace ohkl {

//! IDataReader for Nexus files. Base class for NexusDataReader and FakeDataReader.

class BaseNexusDataReader : public IDataReader {
 public:
    BaseNexusDataReader(const std::string& filename);
    bool initRead() override;

    BaseNexusDataReader() = delete;
    BaseNexusDataReader(const BaseNexusDataReader& other) = delete;
    ~BaseNexusDataReader() override = default;

    BaseNexusDataReader& operator=(const BaseNexusDataReader& other) = delete;

    virtual void open() override;
    virtual void close() override;


 protected:
    std::unique_ptr<H5::H5File> _file;
    std::unique_ptr<H5::DataSet> _dataset;
    std::unique_ptr<H5::DataSpace> _space;
    std::unique_ptr<H5::DataSpace> _memspace;
};

} // namespace ohkl

#endif // OHKL_CORE_LOADER_NEXUSMETADATAREADER_H
