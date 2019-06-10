//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5DataReader.h
//! @brief     Defines class HDF5DataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_HDF5DATAREADER_H
#define CORE_LOADER_HDF5DATAREADER_H

#include "core/loader/HDF5MetaDataReader.h"

namespace nsx {

//! Read data from HDF5 format
class HDF5DataReader : public HDF5MetaDataReader {

public:
    HDF5DataReader() = delete;

    HDF5DataReader(const HDF5DataReader& other) = delete;

    // Note that we need this constructor explicitly defined for SWIG.
    HDF5DataReader(const std::string& filename, Diffractometer* diffractometer);

    ~HDF5DataReader() = default;

    HDF5DataReader& operator=(const HDF5DataReader& other) = delete;

    Eigen::MatrixXi data(size_t frame) final;
};

} // namespace nsx

#endif // CORE_LOADER_HDF5DATAREADER_H
