//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/RawDataReader.h
//! @brief     Defines class RawDataReader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_RAWDATAREADER_H
#define OHKL_CORE_LOADER_RAWDATAREADER_H

#include "base/utils/Path.h" // fileBasename
#include "core/data/DataSet.h"
#include "core/loader/IDataReader.h" // inherits from
#include "core/raw/DataKeys.h"
#include <fstream>
#include <stdexcept>

namespace ohkl {

//! Minimal meta data set, to supplement binary raw data in RawDataReader.
struct DataReaderParameters {
    std::string dataset_name = kw_datasetDefaultName;
    double wavelength = 0.0;
    double delta_omega = 0.0;
    double delta_chi = 0.0;
    double delta_phi = 0.0;
    bool swap_endian = true;
    double baseline = 0.0;
    double gain = 1.0;
    void LoadDataFromFile(std::string file);
    void log(const Level& level) const;
};

struct RawDataReaderParameters : public DataReaderParameters {
    bool row_major = true;
    std::size_t bpp = 2;
    void log(const Level& level) const;
};

//! IDataReader for raw binary data.

class RawDataReader : public IDataReader {
 public:
    RawDataReader(const RawDataReader& other) = delete;

    //! Construct a dataset from the list of files, with the give metadata.
    //! @param delta_chi per-frame change in chi axis of sample goniometer
    //! @param delta_omega per-frame change in omega axis of sample goniometer
    //! @param delta_phi per-frame change in phi axis of sample goniometer
    //! @param rowMajor determines if data is stored in row-major format (column //! major otherwise)
    //! @param swapEndian determines whether to swap the endianness of the input data
    //! @param bpp number of bytes per pixel
    RawDataReader();

    ~RawDataReader() = default;

    void addFrame(const std::string& filename);

    //! Open the file(s)
    void open() final;

    //! Close the file(s)
    void close() final;

    virtual bool initRead() override;

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

    std::size_t _length = 0;

    std::vector<char> _data;
};

template <typename T_>
Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> RawDataReader::matrixFromData() const
{
    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    assert(sizeof(T_) * nrows * ncols == _length);

    if (_parameters.row_major) {
        Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> frame;
        frame.resize(nrows, ncols);
        memcpy(&frame(0, 0), &_data[0], _length);
        return frame;
    } else {
        Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> frame;
        frame.resize(nrows, ncols);
        memcpy(&frame(0, 0), &_data[0], _length);
        return frame;
    }
}

} // namespace ohkl

#endif // OHKL_CORE_LOADER_RAWDATAREADER_H
