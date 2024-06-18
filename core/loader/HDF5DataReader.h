//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/HDF5DataReader.h
//! @brief     Defines class HDF5DataReader (template)
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_HDF5DATAREADER_H
#define OHKL_CORE_LOADER_HDF5DATAREADER_H

#include "core/loader/BaseHDF5DataReader.h" // inherits from

namespace ohkl {

//! Read data from HDF5 format.
class HDF5DataReader : public BaseHDF5DataReader {
 public:
    HDF5DataReader() = delete;

    HDF5DataReader(const HDF5DataReader& other) = delete;

    // Note that we need this constructor explicitly defined for SWIG.
    HDF5DataReader(const std::string& filename);

    ~HDF5DataReader() = default;

    HDF5DataReader& operator=(const HDF5DataReader& other) = delete;

    Eigen::MatrixXi data(size_t frame) override final;
};

} // namespace ohkl

#endif // OHKL_CORE_LOADER_HDF5DATAREADER_H
