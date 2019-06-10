//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/RawDataReader.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_RAWDATAREADER_H
#define CORE_LOADER_RAWDATAREADER_H

#include "core/loader/IDataReader.h"

namespace nsx {

struct RawDataReaderParameters {

    double wavelength = 1.0;
    double delta_omega = 1.0;
    double delta_chi = 1.0;
    double delta_phi = 1.0;
    bool row_major = false;
    bool swap_endian = false;
    size_t bpp = 2;
};

//! \brief Class to detector counts from raw binary data.
class RawDataReader : public IDataReader {

public:
    RawDataReader() = delete;
    RawDataReader(const RawDataReader& other) = delete;

    //! Construct a dataset from the list of files, with the give metadata.
    //! \param delta_chi per-frame change in chi axis of sample goniometer
    //! \param delta_omega per-frame change in omega axis of sample goniometer
    //! \param delta_phi per-frame change in phi axis of sample goniometer
    //! \param rowMajor determines if data is stored in row-major format (column
    //! major otherwise) \param swapEndian determines whether to swap the
    //! endianness of the input data \param bpp is the number of bytes per pixel
    RawDataReader(const std::string& filename, Diffractometer* diffractometer);

    ~RawDataReader() = default;

    void addFrame(const std::string& filename);

    //! Open the file(s)
    void open() final;

    //! Close the file(s)
    void close() final;

    //! Read a single frame
    Eigen::MatrixXi data(size_t frame) final;

    const RawDataReaderParameters& parameters() const;
    void setParameters(const RawDataReaderParameters& parameters);

    //! Swap enddianness of the data
    void swapEndian();

private:
    template <typename T_> Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> matrixFromData() const;

private:
    std::vector<std::string> _filenames;

    RawDataReaderParameters _parameters;

    size_t _length;

    std::vector<char> _data;
};

template <typename T_>
Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> RawDataReader::matrixFromData() const
{
    assert(sizeof(T_) * _nRows * _nCols == _length);

    if (_parameters.row_major) {
        Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> frame;
        frame.resize(_nRows, _nCols);
        memcpy(&frame(0, 0), &_data[0], _length);
        return frame;
    } else {
        Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> frame;
        frame.resize(_nRows, _nCols);
        memcpy(&frame(0, 0), &_data[0], _length);
        return frame;
    }
}

} // namespace nsx

#endif // CORE_LOADER_RAWDATAREADER_H
