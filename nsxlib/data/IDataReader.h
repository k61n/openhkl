#ifndef IDATAREADER_H_
#define IDATAREADER_H_

#include <string>
#include <vector>
#include <memory>

#include <Eigen/Dense>

#include "../data/MetaData.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/InstrumentState.h"

namespace nsx {

class IDataReader {

public:

    IDataReader(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer);

    virtual ~IDataReader()=default;

    //! Get the file handle.
    virtual void open()=0;

    //! Close file and release handle
    virtual void close()=0;

    virtual Eigen::MatrixXi getData(size_t frame)=0;

    InstrumentState getState(size_t frame) const;

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
    void saveHDF5(const std::string& filename);

protected:

    MetaData _metadata;

    std::shared_ptr<Diffractometer> _diffractometer;

    std::size_t _nFrames;

    std::size_t _nRows;

    std::size_t _nCols;

    std::vector<InstrumentState> _states;

    std::size_t _fileSize;

    bool _isOpened;

};

} // end namespace nsx

#endif /* IDATAREADER_H_ */
