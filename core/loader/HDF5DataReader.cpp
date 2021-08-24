//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/HDF5DataReader.cpp
//! @brief     Defines class HDF5DataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/HDF5DataReader.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"

#include <Eigen/Dense>
#include <H5Cpp.h>
#include <string>


namespace nsx {

HDF5DataReader::HDF5DataReader(const std::string& filename) : BaseHDF5DataReader(filename) {}

Eigen::MatrixXi HDF5DataReader::data(std::size_t frame)
{
    const std::size_t nframes = _dataset_out->nFrames(),
        nrows = _dataset_out->nRows(),
        ncols = _dataset_out->nCols();

    if (frame >= nframes)
        throw std::range_error("Frame index " + std::to_string(frame)
                               + " is out of bounds (" + std::to_string(nframes) + ")");

    // Open HDF5 file (does nothing if already opened)
    open();

    // HDF5 specification requires row-major storage
    using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    RowMatrixXi m(nrows, ncols);
    const hsize_t count_1frm[3] = {1, nrows, ncols}; // a single frame
    const hsize_t offset[3] = {frame, 0, 0};
    _space->selectHyperslab(H5S_SELECT_SET, count_1frm, offset);
    _dataset->read(m.data(), H5::PredType::NATIVE_INT32, *_memspace, *_space);
    // return copy as MatrixXi (col-major)
    return Eigen::MatrixXi(m);
}

} // namespace nsx
