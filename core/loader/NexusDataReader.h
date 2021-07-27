//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/NexusDataReader.h
//! @brief     Defines class NexusDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_NEXUSDATAREADER_H
#define NSX_CORE_LOADER_NEXUSDATAREADER_H

#include "core/loader/BaseNexusDataReader.h" // inherits from

namespace nsx {

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

} // namespace nsx

#endif // NSX_CORE_LOADER_NEXUSDATAREADER_H
