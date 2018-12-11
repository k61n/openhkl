
#pragma once

#include <string>

#include "H5Cpp.h"
#include "IDataReader.h"

namespace nsx {

//! \brief Read the experiment metadata from and HDF file.
class HDF5MetaDataReader : public IDataReader {

public:
    HDF5MetaDataReader() = delete;

    HDF5MetaDataReader(const HDF5MetaDataReader& other);

    HDF5MetaDataReader(const std::string& filename, Diffractometer* instrument);

    virtual ~HDF5MetaDataReader();

    HDF5MetaDataReader& operator=(const HDF5MetaDataReader& other);

    IDataReader* clone() const = 0;

    virtual void open() override;

    virtual void close() override;

protected:
    void init();

    std::unique_ptr<H5::H5File> _file;

    std::unique_ptr<H5::DataSet> _dataset;

    std::unique_ptr<H5::DataSpace> _space;

    std::unique_ptr<H5::DataSpace> _memspace;
};

} // end namespace nsx
