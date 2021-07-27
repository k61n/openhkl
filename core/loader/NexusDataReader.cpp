//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/NexusDataReader.cpp
//! @brief     Implements class NexusDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/NexusDataReader.h"

#include "base/parser/BloscFilter.h"
#include "core/data/DataSet.h"

#include <memory>

namespace nsx {

NexusDataReader::NexusDataReader(const std::string& filename)
    : BaseNexusDataReader(filename) {}

Eigen::MatrixXi NexusDataReader::data(size_t frame)
{
    // std::cout << "NexusDataReader::data, this=" << std::hex << (void*)this << ", frame=" <<
    // std::dec << frame << std::endl;

    if (!_isOpened)
        open();

    const std::size_t nframes = _dataset_out->nFrames(),
        nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    if (frame >= nframes)
        throw std::runtime_error("Requested invalid frame " + std::to_string(frame));

    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> m(nrows, ncols);

    hsize_t count[3] = {1, ncols, nrows};
    hsize_t offset[3] = {frame, 0, 0};
    _space->selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
    _dataset->read(m.data(), H5::PredType::NATIVE_INT32, *_memspace, *_space);

    return Eigen::MatrixXi(m);
}

} // namespace nsx
