//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/ExperimentDataReader.cpp
//! @brief     Implements class ExperimentDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/ExperimentDataReader.h"

#include "base/parser/BloscFilter.h"
#include "core/instrument/Diffractometer.h"

#include <memory>

namespace nsx {

ExperimentDataReader::ExperimentDataReader(
    const std::string& file_name, const std::string& group_name, Diffractometer* instrument)
    : ExperimentMetaReader(file_name, group_name, instrument)
{
}

Eigen::MatrixXi ExperimentDataReader::data(size_t frame)
{
    if (!_isOpened)
        open();
    // HDF5 specification requires row-major storage
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> m(_nRows, _nCols);

    hsize_t count[3] = {1, _nRows, _nCols};
    hsize_t offset[3] = {frame, 0, 0};
    _space->selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
    _dataset->read(m.data(), H5::PredType::NATIVE_INT32, *_memspace, *_space);
    // return copy as MatrixXi (which could be col-major)
    return Eigen::MatrixXi(m);
}

} // namespace nsx
