#ifndef CORE_DATA_HDF5DATAREADER_H
#define CORE_DATA_HDF5DATAREADER_H

#include <string>

#include "H5Cpp.h"
#include "HDF5MetaDataReader.h"

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

} // end namespace nsx

#endif // CORE_DATA_HDF5DATAREADER_H
