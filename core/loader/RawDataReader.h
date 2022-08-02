//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/loader/RawDataReader.h
//! @brief     Defines class RawDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_RAWDATAREADER_H
#define OHKL_CORE_LOADER_RAWDATAREADER_H

#include "core/data/DataSet.h"
#include "core/loader/IDataReader.h" // inherits from
#include "core/raw/DataKeys.h"
#include "base/utils/Path.h" // fileBasename
#include <fstream>
#include <stdexcept>

namespace ohkl {

//! Minimal meta data set, to supplement binary raw data in RawDataReader.

struct RawDataReaderParameters {
    std::string dataset_name = kw_datasetDefaultName;
    double wavelength = 0.0;
    double delta_omega = 0.0;
    double delta_chi = 0.0;
    double delta_phi = 0.0;
    bool row_major = true;
    bool swap_endian = true;
    std::size_t bpp = 2;
    double baseline = 0.0;
    double gain = 1.0;

    void LoadDataFromFile(std::string file)
    {
        dataset_name = fileBasename(file);

        std::size_t pos1 = file.find_last_of("/");
        std::size_t pos0 = (file.substr(0, pos1 - 1)).find_last_of("/");
        std::size_t pos2 = file.find_last_of(".");

        if (pos1 == std::string::npos || pos0 == std::string::npos || pos2 == std::string::npos)
            return;

        std::string dir = "data_" + file.substr(pos0 + 1, pos1 - pos0 - 1);
        std::string readme = file.substr(0, pos1 + 1) + dir + ".readme";

        std::ifstream fin(readme.c_str(), std::ios::in);

        if (fin.is_open() || fin.good()) {
            fin.seekg(0, std::ios::end);
            auto fsize = fin.tellg();
            fin.seekg(0, std::ios::beg);

            if (fsize > 0) {
                std::string buffer;
                buffer.resize(fsize);
                fin.read(buffer.data(), fsize);
                fin.close();

                std::remove_if(buffer.begin(), buffer.end(), isspace);

                auto omega_pos = buffer.find("Omegarange:");
                if (omega_pos != std::string::npos) {
                    auto nl_pos = buffer.find(";", omega_pos);
                    std::string a = buffer.substr(omega_pos + 11, nl_pos - 1);
                    delta_omega = std::stod(a);
                }

                auto lambda_pos = buffer.find("Lambda:");
                if (lambda_pos != std::string::npos) {
                    auto nl_pos = buffer.find(";", lambda_pos);
                    std::string b = buffer.substr(lambda_pos + 7, nl_pos - 1);
                    wavelength = std::stod(b);
                }
            }
        }
    }
};

//! IDataReader for raw binary data.

class RawDataReader : public IDataReader {
 public:
    RawDataReader(const RawDataReader& other) = delete;

    //! Construct a dataset from the list of files, with the give metadata.
    //! @param delta_chi per-frame change in chi axis of sample goniometer
    //! @param delta_omega per-frame change in omega axis of sample goniometer
    //! @param delta_phi per-frame change in phi axis of sample goniometer
    //! @param rowMajor determines if data is stored in row-major format (column
    //! major otherwise) @param swapEndian determines whether to swap the
    //! endianness of the input data @param bpp is the number of bytes per pixel
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
