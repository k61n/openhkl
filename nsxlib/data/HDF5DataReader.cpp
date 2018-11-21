#include <memory>

#include "blosc.h"

#include "BloscFilter.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "EigenToVector.h"
#include "Gonio.h"
#include "HDF5DataReader.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace nsx {

HDF5DataReader::HDF5DataReader(const std::string& filename, Diffractometer* instrument):
    HDF5MetaDataReader(filename, instrument)
{
}

IDataReader* HDF5DataReader::clone() const
{
    return new HDF5DataReader(*this);
}

Eigen::MatrixXi HDF5DataReader::data(size_t frame)
{
    if (!_isOpened) {
        open();
    }
    // HDF5 specification requires row-major storage
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> m(_nRows,_nCols);

    hsize_t count[3]={1,_nRows,_nCols};
    hsize_t offset[3]={frame,0,0};
    _space->selectHyperslab(H5S_SELECT_SET,count,offset,nullptr,nullptr);
    _dataset->read(m.data(),H5::PredType::NATIVE_INT32,*_memspace,*_space);
    // return copy as MatrixXi (which could be col-major)
    return Eigen::MatrixXi(m);
}

} // end namespace nsx
