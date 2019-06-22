//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/TiffDataReader.h
//! @brief     Defines class TiffDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_TIFFDATAREADER_H
#define CORE_LOADER_TIFFDATAREADER_H

#include <tiffio.h>

#include "core/raw/IDataReader.h" \\ inherits from

namespace nsx {

//! IDataReader for detector image from Tiff file.

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

} // namespace nsx

#endif // CORE_LOADER_TIFFDATAREADER_H
