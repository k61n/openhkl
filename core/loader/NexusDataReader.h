//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/NexusDataReader.h
//! @brief     Defines class NexusDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_NEXUSDATAREADER_H
#define OHKL_CORE_LOADER_NEXUSDATAREADER_H

#include "core/loader/BaseNexusDataReader.h" // inherits from

namespace ohkl {

//! Read data from Nexus format.

class NexusDataReader : public BaseNexusDataReader {
 public:
    NexusDataReader() = delete;

    NexusDataReader(const NexusDataReader& other) = delete;

    // Note that we need this constructor explicitly defined for SWIG.
    NexusDataReader(const std::string& filename);

    ~NexusDataReader() = default;

    NexusDataReader& operator=(const NexusDataReader& other) = delete;

    Eigen::MatrixXi data(size_t frame) final;
};

} // namespace ohkl

#endif // OHKL_CORE_LOADER_NEXUSDATAREADER_H
