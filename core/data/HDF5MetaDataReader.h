
#pragma once

#include <string>

#include "H5Cpp.h"
#include "IDataReader.h"

namespace nsx {

//! \brief Read the experiment metadata from and HDF file.
class HDF5MetaDataReader : public IDataReader {

public:
  HDF5MetaDataReader() = delete;

  HDF5MetaDataReader(const HDF5MetaDataReader &other) = delete;

  HDF5MetaDataReader(const std::string &filename, Diffractometer *instrument);

  ~HDF5MetaDataReader() override = default;

  HDF5MetaDataReader &operator=(const HDF5MetaDataReader &other) = delete;

  virtual void open() override;

  virtual void close() override;

protected:
  std::unique_ptr<H5::H5File> _file;

  std::unique_ptr<H5::DataSet> _dataset;

  std::unique_ptr<H5::DataSpace> _space;

  std::unique_ptr<H5::DataSpace> _memspace;
};

} // end namespace nsx
