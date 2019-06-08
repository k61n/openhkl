//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/HDF5DataReader.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <memory>

#include "blosc.h"

#include "core/data/BloscFilter.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/data/EigenToVector.h"
#include "core/axes/Gonio.h"
#include "core/data/HDF5DataReader.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/utils/Units.h"

namespace nsx {

HDF5DataReader::HDF5DataReader(const std::string& filename, Diffractometer* instrument)
    : HDF5MetaDataReader(filename, instrument)
{
}

Eigen::MatrixXi HDF5DataReader::data(size_t frame)
{
    if (!_isOpened) {
        open();
    }
    // HDF5 specification requires row-major storage
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> m(_nRows, _nCols);

    hsize_t count[3] = {1, _nRows, _nCols};
    hsize_t offset[3] = {frame, 0, 0};
    _space->selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
    _dataset->read(m.data(), H5::PredType::NATIVE_INT32, *_memspace, *_space);
    // return copy as MatrixXi (which could be col-major)
    return Eigen::MatrixXi(m);
}

} // end namespace nsx
