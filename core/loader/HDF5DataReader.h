//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5DataReader.h
//! @brief     Defines class HDF5DataReader (template)
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_HDF5DATAREADER_H
#define NSX_CORE_LOADER_HDF5DATAREADER_H

#include "core/loader/HDF5MetaDataReader.h" // inherits from
#include "core/instrument/Diffractometer.h"
#include "core/raw/DataKeys.h" // kw_datasetDefaultName


namespace nsx {

//! Read data from HDF5 format.
class HDF5DataReader : public HDF5MetaDataReader {
 public:
    HDF5DataReader() = delete;

    HDF5DataReader(const HDF5DataReader& other) = delete;

    // Note that we need this constructor explicitly defined for SWIG.
    HDF5DataReader(const std::string& filename, Diffractometer* diffractometer,
                   std::string dataset_name = nsx::kw_datasetDefaultName);

    ~HDF5DataReader() = default;

    HDF5DataReader& operator=(const HDF5DataReader& other) = delete;

    Eigen::MatrixXi data(size_t frame) override final;
};

} // namespace nsx

#endif // NSX_CORE_LOADER_HDF5DATAREADER_H
