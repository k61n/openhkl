//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5MetaDataReader.h
//! @brief     Defines class HDF5MetaDataReader (specialized templates)
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/HDF5MetaDataReader.h"

#include <string>

namespace nsx {

// Template specializations

// Legacy-reader group keys

template <> std::string HDF5MetaDataReader<LegacyReader>::_metaKey(const std::string& _) const
{
    return "Experiment";
}

template <> std::string HDF5MetaDataReader<LegacyReader>::_infoKey(const std::string& _) const
{
    return "Info";
}

template <> std::string HDF5MetaDataReader<LegacyReader>::_detectorKey(const std::string& _) const
{
    return "Data/Scan/Detector";
}

template <> std::string HDF5MetaDataReader<LegacyReader>::_sampleKey(const std::string& _) const
{
    return "Data/Scan/Sample";
}

template <> std::string HDF5MetaDataReader<LegacyReader>::_dataKey(const std::string& _) const
{
    return "Data/Counts";
}


// Experiment-reader group keys
template <> std::string HDF5MetaDataReader<ExperimentReader>::_metaKey(const std::string& group_name) const
{
    return "DataCollections/" + group_name + "/Meta";
}

template <> std::string HDF5MetaDataReader<ExperimentReader>::_infoKey(const std::string& group_name) const
{
    return "DataCollections/" + group_name + "/Info";
}

template <> std::string HDF5MetaDataReader<ExperimentReader>::_detectorKey(const std::string& group_name) const
{
    return "DataCollections/" + group_name + "/Detector";
}

template <> std::string HDF5MetaDataReader<ExperimentReader>::_sampleKey(const std::string& group_name) const
{
    return "DataCollections/" + group_name + "/Sample";
}

template <> std::string HDF5MetaDataReader<ExperimentReader>::_dataKey(const std::string& group_name) const
{
    return "DataCollections/" + group_name + "/" + group_name;
}
//-----------------------------------------------------------------------------80


} // namespace nsx
