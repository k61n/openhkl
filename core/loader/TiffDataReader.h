//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/TiffDataReader.h
//! @brief     Defines class TiffDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_LOADER_TIFFDATAREADER_H
#define OHKL_CORE_LOADER_TIFFDATAREADER_H

#include "base/utils/Logger.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/DataKeys.h"

#include "tiffio.h"
#include <filesystem>
#include <string>
#include <vector>

namespace ohkl {
struct TiffDataReaderParameters : DataReaderParameters {
    //! Side of square grid over which to average during rebinning
    int rebin_size = 1;
    //! Bits per pixel (not to be confused with bpp, bytes per pixel)
    int bits_per_pixel = 1;

    void log(const Level& level) const;
};

/*
    struct to hold Tiff Tags with meta data for an opened Tiff file
    not all possible tiff tags are covered here only those which are used
 */
struct TiffMetadata {
    unsigned int image_width = -1; //!< Image width in px
    unsigned int image_length = -1; //!< Image height in px
    unsigned short bits_per_pixel = -1; //!< Number of bits per pixel
    unsigned short compression = -1; //!< Whether tiff file uses a compression mechanism
    unsigned short photometric = -1; //!< Greyscale or colored tiff
    unsigned short planar_config = -1; //!< Whether pixels stored Chunky or planar format
    unsigned int npixels = -1; //!< Number of pixels

    void log(const Level& level) const;
};

class TiffDataReader : public IDataReader {
 public:
    //! Constructor
    TiffDataReader();
    //! Constructor
    TiffDataReader(const TiffDataReader& other) = delete;
    //! Deconstructor
    ~TiffDataReader();
    //! add a frame/file to the TiffDataReader
    void addFrame(const std::string& filename);
    //! Open Files
    void open() override{};
    //! Close Files
    void close() override{};
    //! initRead
    bool initRead() override;
    //! access data
    Eigen::MatrixXi data(size_t frame) override final;
    //! set Parameters
    void setParameters(const TiffDataReaderParameters& parameters);
    //! Swap Endians
    void swapEndian();
    //! Read Tags from Tiff file. Will throw error if necessary tag is not available
    void readTags(TIFF*);
    //! Read data from the opened Tiff file
    void readData();
    //! Will rebin data to target dimensions
    void rebin(int rebins);
    //! this method is scanning a given tiff file for its meta data tags without loading data
    TiffMetadata scanFile(std::string filename);
    //! will register file dimensions as meta data
    void registerFileDimension(std::string filename);
    //! read image (file) resolutions of a given vector of tiff files an return them
    std::vector<std::pair<int, int>> readFileResolutions(std::vector<std::string> filenames);
    //! returns all bpp values from given filenames
    std::vector<int> readFileBitDepths(std::vector<std::string> filenames);

 private:
    template <typename T_> Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> matrixFromData() const;
    std::vector<std::string> _filenames;
    std::vector<std::string> _file_resolutions;
    TiffDataReaderParameters _parameters;
    std::size_t _length_after_rebin = 0;
    std::vector<char> _data;
    std::vector<unsigned short> _buffer;

    TiffMetadata _tiff_meta_data;

    TIFF* _tiff;
};

template <typename T_>
Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> TiffDataReader::matrixFromData() const
{
    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    assert(sizeof(T_) * nrows * ncols == _length_after_rebin);

    // as far as I can tell row major will always be the same for TIFF files
    Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> frame;
    frame.resize(nrows, ncols);
    memcpy(&frame(0, 0), &_data[0], _length_after_rebin);
    return frame;
}

} // namespace ohkl
#endif
