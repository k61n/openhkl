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

#include "base/utils/Logger.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/HDF5MetaDataReader.h" // inherits from


namespace nsx {

//! Read data from HDF5 format.
template <HDF5ReaderType ReaderT>
class HDF5DataReader : public HDF5MetaDataReader<ReaderT> {
 public:
    HDF5DataReader() = delete;

    HDF5DataReader(const HDF5DataReader& other) = delete;

    // Note that we need this constructor explicitly defined for SWIG.
    HDF5DataReader(const std::string& filename, Diffractometer* diffractometer,
                   const std::string& group_name = "");

    ~HDF5DataReader() = default;

    HDF5DataReader& operator=(const HDF5DataReader& other) = delete;

    Eigen::MatrixXi data(size_t frame) override final;
};

//-----------------------------------------------------------------------------80

template <HDF5ReaderType ReaderT>
HDF5DataReader<ReaderT>::HDF5DataReader(
    const std::string& filename, Diffractometer* diffractometer, const std::string& group_name)
    : HDF5MetaDataReader<ReaderT>(filename, diffractometer, group_name)
{
}

template <HDF5ReaderType ReaderT>
Eigen::MatrixXi HDF5DataReader<ReaderT>::data(std::size_t frame)
{
    nsxlog(nsx::Level::Info, __PRETTY_FUNCTION__, ":", "Reading data in frame nr.", frame);

    // NOTE: `this->` is needed due to C++ two-phase name lookup mechanism

    // Open HDF5 file (does nothing if already opened)
    this->open();

    // HDF5 specification requires row-major storage
    using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    RowMatrixXi m(this->_nRows, this->_nCols);
    const hsize_t count_1frm[3] = {1, this->_nRows, this->_nCols}; // a single frame
    const hsize_t offset[3] = {frame, 0, 0};
    this->_space->selectHyperslab(H5S_SELECT_SET, count_1frm, offset);
    this->_dataset->read(m.data(), H5::PredType::NATIVE_INT32, *(this->_memspace), *(this->_space));
    // return copy as MatrixXi (col-major)
    return Eigen::MatrixXi(m);
}


} // namespace nsx

#endif // NSX_CORE_LOADER_HDF5DATAREADER_H
