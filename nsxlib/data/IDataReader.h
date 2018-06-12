#pragma once

#include <string>
#include <vector>

#include <Eigen/Dense>

#include "InstrumentState.h"
#include "InstrumentTypes.h"
#include "MetaData.h"

namespace nsx {

//! \brief Interface used to provide access to detector images and metadata.
class IDataReader {

public:
    //! Construct data reader from given filename and diffractometer
    IDataReader(const std::string& filename, const sptrDiffractometer& diffractometer);
    //! Destructor
    virtual ~IDataReader() = default;
    //! Open the file
    virtual void open() = 0;
    //! Close file and release handle
    virtual void close() = 0;
    //! Return the a detector image
    virtual Eigen::MatrixXi data(size_t frame)=0;
    //! Return the instrument state as read from the metadata
    InstrumentState state(size_t frame) const;
    //! Return the list of sample states associated to the detector images
    const std::vector<std::vector<double>>& sampleStates() const;
    //! Return the list of detector states associated to the detecot images
    const std::vector<std::vector<double>>& detectorStates() const;
    //! Return the metadata in the file
    const MetaData& metadata() const;
    //! Return number of detector images
    size_t nFrames() const;
    //! Return number of columns in each detector image
    size_t nCols() const;
    //! Return number of rows in each detector image
    size_t nRows() const;
    //! Return pointer to diffractometer associated with the data
    sptrDiffractometer diffractometer() const;
    //! Gets the data basename
    std::string basename() const;
    //! Gets the data filename
    std::string filename() const;
    //! True if file is open
    bool isOpened() const;
    //! Returns the size of the file in disk
    std::size_t fileSize() const;//

protected:
    //! Stores the metadata
    MetaData _metadata;
    //! Shared pointer to diffractometer
    sptrDiffractometer _diffractometer;
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
