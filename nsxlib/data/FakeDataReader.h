
#pragma once

#include <string>

#include "H5Cpp.h"

#include "HDF5MetaDataReader.h"
#include "InstrumentTypes.h"

namespace nsx {

//! This class is used to make simulated/faked data behave as if it were real data.
//! This is used to testing purposes.
class FakeDataReader: public HDF5MetaDataReader {

public:

    FakeDataReader() = delete;

    FakeDataReader(const FakeDataReader &other) = delete;

    //! Constructor. Behaves as if it is loaded from filename (which must exist!),
    //! but the frame values may be overwritten arbitrarily (without modifying the file on disk).
    FakeDataReader(const std::string& filename, Diffractometer *instrument);

    ~FakeDataReader() = default;

    FakeDataReader& operator=(const FakeDataReader &other) = delete;

    //! Return the data frame for the given index.
    Eigen::MatrixXi data(size_t frame) final;

    //! Set the value of the data at the given frame
    void setData(size_t frame, const Eigen::MatrixXi& data);

private:

    //! Stored data frames. Could be real or simulated/faked.
    std::vector<Eigen::MatrixXi> _frames;
};

} // end namespace nsx
