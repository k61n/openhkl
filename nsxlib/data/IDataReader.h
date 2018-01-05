#pragma once

#include <string>
#include <vector>

#include <Eigen/Dense>

#include "InstrumentState.h"
#include "InstrumentTypes.h"
#include "MetaData.h"

namespace nsx {

class IDataReader {

public:

    IDataReader(const std::string& filename, const sptrDiffractometer& diffractometer);

    virtual ~IDataReader()=default;

    //! Get the file handle.
    virtual void open()=0;

    //! Close file and release handle
    virtual void close()=0;

    virtual Eigen::MatrixXi getData(size_t frame)=0;

    InstrumentState getState(size_t frame) const;

    const std::vector<ComponentState>& sampleStates() const;
    const std::vector<ComponentState>& detectorStates() const;

    const MetaData& getMetadata() const;

    size_t getNFrames() const;

    size_t getNCols() const;

    size_t getNRows() const;

    const Diffractometer& getDiffractometer() const;

    //! Gets the data basename
    std::string getBasename() const;

    //! Gets the data filename
    std::string getFilename() const;

    //! True if file is open
    bool isOpened() const;

    //! Returns the size of the file in disk
    std::size_t getFileSize() const;//

    //! Save the file in HDF5 format
    //void saveHDF5(const std::string& filename);

protected:

    MetaData _metadata;

    sptrDiffractometer _diffractometer;

    std::size_t _nFrames;

    std::size_t _nRows;

    std::size_t _nCols;

    std::vector<ComponentState> _sampleStates;

    std::vector<ComponentState> _detectorStates;

    std::size_t _fileSize;

    bool _isOpened;

};

} // end namespace nsx
