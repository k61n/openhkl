#ifndef CORE_DATA_TIFFDATAREADER_H
#define CORE_DATA_TIFFDATAREADER_H

#include <tiffio.h>

#include "IDataReader.h"

namespace nsx {

//! Read detector image from Tiff file
class TiffDataReader : public IDataReader {

public:
    TiffDataReader() = delete;

    TiffDataReader(const TiffDataReader& other) = delete;

    TiffDataReader(const std::string& filename, Diffractometer* diffractometer);

    ~TiffDataReader() = default;

    TiffDataReader& operator=(const TiffDataReader& other) = delete;

    void open() final;

    void close() final;

    Eigen::MatrixXi data(std::size_t frame) final;

private:
    //! Type of encoding for each pixel.
    uint16 _bits;

    //! The handler to the TIFF file
    TIFF* _file;
};

} // end namespace nsx

#endif // CORE_DATA_TIFFDATAREADER_H
