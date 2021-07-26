//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/raw/IDataReader.h
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

#include "core/instrument/InstrumentState.h"
#include "core/raw/MetaData.h"
#include <vector>
#include <string>

namespace nsx {

//! Pure virtual base class of data readers that provide access to detector images and metadata.

class DataSet;

class IDataReader {

 public:

    //! Status of reader initialization
    bool isInitialized = false;

    IDataReader(const std::string& filename, Diffractometer* diffractometer);
    virtual ~IDataReader() = 0;

    //! Initialize reading from the filename
    virtual bool initRead();

    //! Open the file
    virtual void open() = 0;

    //! Close file and release handle
    virtual void close() = 0;

    //! Returns the a detector image
    virtual Eigen::MatrixXi data(size_t frame) = 0;

    //! Returns the instrument state as read from the metadata
    InstrumentState state(size_t frame) const;

    //! Returns the list of sample states associated to the detector images
    const std::vector<std::vector<double>>& sampleStates() const;

    //! Returns the list of detector states associated to the detecot images
    const std::vector<std::vector<double>>& detectorStates() const;

    //! True if file is open
    bool isOpened() const;

    //! Set the DataSet where the data will be stored (allowed only once)
    void setDataSet(DataSet* dataset_out);

    //! Check if the reader is successfully initialized
    void checkInit();

 protected:
    IDataReader() = delete;

    IDataReader(const IDataReader& other) = delete;

    IDataReader& operator=(const IDataReader& other) = delete;

    //! Vector of sample states
    std::vector<std::vector<double>> _sampleStates;

    //! Vector of detector states
    std::vector<std::vector<double>> _detectorStates;

    //! Status of file handle
    bool _isOpened;

    //! Filename
    std::string _filename;

    //! Destination DataSet where the data will be stored
    DataSet* _dataset_out = nullptr;

};

} // namespace nsx

#endif // NSX_CORE_RAW_IDATAREADER_H
