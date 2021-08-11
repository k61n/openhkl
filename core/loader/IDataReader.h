//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/IDataReader.h
//! @brief     Defines class IDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_RAW_IDATAREADER_H
#define NSX_CORE_RAW_IDATAREADER_H

#include "core/raw/MetaData.h"
#include <vector>
#include <string>
#include <Eigen/Dense>

namespace nsx {

//! Pure virtual base class of data readers that provide access to detector images and metadata.

class DataSet;

class IDataReader {

 public:

    //! Status of reader initialization
    bool isInitialized = false;

    IDataReader(const std::string& filename);
    virtual ~IDataReader() = 0;

    //! Initialize reading from the filename
    virtual bool initRead();

    //! Open the file
    virtual void open() = 0;

    //! Close file and release handle
    virtual void close() = 0;

    //! Returns the a detector image
    virtual Eigen::MatrixXi data(size_t frame) = 0;

    //! True if file is open
    bool isOpened() const;

    //! Set the DataSet where the data will be stored (allowed only once)
    void setDataSet(DataSet* dataset_out);

    //! Check if the reader is successfully initialized
    void checkInit();

    //! Return the NSX (HDF5) filepath associated with the dataset (if any)
    virtual std::string NSXfilepath() const;

 protected:
    IDataReader() = delete;

    IDataReader(const IDataReader& other) = delete;

    IDataReader& operator=(const IDataReader& other) = delete;

    //! Status of file handle
    bool _isOpened;

    //! Filename
    std::string _filename;

    //! Destination DataSet where the data will be stored
    DataSet* _dataset_out = nullptr;

};

} // namespace nsx

#endif // NSX_CORE_RAW_IDATAREADER_H
