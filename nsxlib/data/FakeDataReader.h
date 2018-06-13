
#pragma once

#include <string>

#include "H5Cpp.h"

#include "HDF5MetaDataReader.h"
#include "InstrumentTypes.h"

namespace nsx {

class FakeDataReader: public HDF5MetaDataReader
{

public:
    FakeDataReader(const std::string& filename, sptrDiffractometer instrument);
    
    Eigen::MatrixXi data(size_t frame) override;
    void setData(size_t frame, const Eigen::MatrixXi& data);

private:
    std::vector<Eigen::MatrixXi> _frames;
};

} // end namespace nsx
