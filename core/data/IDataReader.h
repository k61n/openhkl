//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/IDataReader.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_DATA_IDATAREADER_H
#define CORE_DATA_IDATAREADER_H

#include <string>
#include <vector>

#include <Eigen/Dense>

#include "InstrumentState.h"
#include "InstrumentTypes.h"
#include "MetaData.h"

namespace nsx {

class Diffractometer;

//! \brief Interface used to provide access to detector images and metadata.
class IDataReader {

public:
    //! Construct data reader from given filename and diffractometer
    IDataReader(const std::string& filename, Diffractometer* diffractometer);
    virtual ~IDataReader() = 0;

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

    //! Returns the metadata in the file
    const MetaData& metadata() const;

    //! Returns the metadata in the file
    MetaData& metadata();

    //! Returns number of detector images
    size_t nFrames() const;

    //! Returns number of columns in each detector image
    size_t nCols() const;

    //! Returns number of rows in each detector image
    size_t nRows() const;

    //! Returns a const pointer to the diffractometer bound to the data
    const Diffractometer* diffractometer() const;

    //! Returns a pointer to the diffractometer bound to the data
    Diffractometer* diffractometer();

    //! Gets the data basename
    std::string basename() const;

    //! Gets the data filename
    std::string filename() const;

    //! True if file is open
    bool isOpened() const;

    //! Returns the size of the file in disk
    std::size_t fileSize() const; //

protected:
    IDataReader() = delete;

    IDataReader(const IDataReader& other) = delete;

    IDataReader& operator=(const IDataReader& other) = delete;

    //! Stores the metadata
    MetaData _metadata;

    //! A pointer to the diffractometer. The actual resource is not owned by this
    //! object which is just a borrower.
    Diffractometer* _diffractometer;

    //! Number of frames of data
    std::size_t _nFrames;

    //! Number of rows
    std::size_t _nRows;

    //! Number of columns
    std::size_t _nCols;

    //! Vector of sample states
    std::vector<std::vector<double>> _sampleStates;

    //! Vector of detector states
    std::vector<std::vector<double>> _detectorStates;

    //! Total size of file
    std::size_t _fileSize;

    //! Status of file handle
    bool _isOpened;
};

} // end namespace nsx

#endif // CORE_DATA_IDATAREADER_H
