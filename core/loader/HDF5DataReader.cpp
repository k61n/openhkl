//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5DataReader.cpp
//! @brief     Implements class HDF5DataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/HDF5DataReader.h"
#include "core/instrument/Diffractometer.h"

#include <memory>

namespace nsx {

HDF5DataReader::HDF5DataReader(const std::string& filename, Diffractometer* diffractometer)
    : HDF5MetaDataReader(filename, diffractometer)
{
}

Eigen::MatrixXi HDF5DataReader::data(const std::size_t frame)
{
    // Open HDF5 file (does nothing if already opened)
    open();

    // HDF5 specification requires row-major storage
    using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    RowMatrixXi m(_nRows, _nCols);
    const hsize_t count_1frm[3] = {1, _nRows, _nCols}; // a single frame
    const hsize_t offset[3] = {frame, 0, 0};
    _space->selectHyperslab(H5S_SELECT_SET, count_1frm, offset);
    _dataset->read(m.data(), H5::PredType::NATIVE_INT32, *_memspace, *_space);
    // return copy as MatrixXi (col-major)
    return Eigen::MatrixXi(m);
}

} // namespace nsx
