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
struct TiffDataReaderParameters {
    std::string dataset_name = kw_datasetDefaultName;
    double wavelength = 0.0;
    double delta_omega = 0.0;
    double delta_chi = 0.0;
    double delta_phi = 0.0;
    bool swap_endian = false;
    double baseline = 0.0;
    double gain = 1.0;
    // we use this to store the number of pixel which are rebinned
    // we always rebin symmetrically and thus just store the number of
    // total rebinned pixels 2x2 -> 4, 4x4 -> 16
    int data_binnning;

    /**
     * This loads .readme file with additional meta date from the folder in which the data is placed
     * This files can be created by the instrumentalist who is providing these data files and are
     * thought to help the user to load data files into projects without entering known parameters.
     */
    void LoadDataFromFile(std::string file);
};

/*
    struct to hold Tiff Tags with meta data for an opened Tiff file
    not all possible tiff tags are covered here only those which are used
 */
struct tiff_file_metadata {
    uint32 _width = -1; // image width in px
    uint32 _image_length = -1; // image height in px
    uint16 _bits_per_pixel = -1;
    uint16 _compression = -1; // does tiff file uses a compression mechanism
    uint16 _photometric = -1; // black white or colored tiff files
    uint16 _planar_config = -1; // how are the pixels stored Chunky format or planar format
    uint32 _npixels = -1; // number of pixels

    friend std::ostream& operator<<(std::ostream& oss, const tiff_file_metadata& o)
    {
        oss << " * * * TIF FILE META DATA * * *\n"
            << std::setfill('.') << "\nimage width: " << std::setw(20) << o._width
            << "\nimage height: " << std::setw(19) << o._image_length << "\nbpp: " << std::setw(22)
            << o._bits_per_pixel << "\ncompression: " << std::setw(13) << o._compression
            << "\nphotometric: " << std::setw(13) << o._photometric
            << "\nplanar config: " << std::setw(11) << o._planar_config;
        return oss;
    }
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
    //! Set detector width in px. this is the target width which data is binned to
    void setTargetWidth(int nwidth) { _n_target_width = nwidth; };
    //! Set detector height in px. this is the target height which data is binned to
    void setTargetHeight(int nheight) { _n_target_height = nheight; };
    //! this method is scanning a given tiff file for its meta data tags without loading data
    tiff_file_metadata scanFile(std::string filename);
    //! will register file dimensions as meta data
    void registerFileDimension(std::string filename);
    //! read image (file) resolutions of a given vector of tiff files an return them
    std::vector<std::string> readFileResolutions(std::vector<std::string> filenames);

 private:
    template <typename T_> Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> matrixFromData() const;
    std::vector<std::string> _filenames;
    std::vector<std::string> _file_resolutions;
    TiffDataReaderParameters _parameters;
    std::size_t _length = 0;
    std::vector<char> _data;
    std::vector<uint16> _buffer;

    tiff_file_metadata _tiff_meta_data;
    int _n_target_width;
    int _n_target_height;

    TIFF* _tiff;
};

template <typename T_>
Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> TiffDataReader::matrixFromData() const
{
    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    assert(sizeof(T_) * nrows * ncols == _length);

    // as far as I can tell row major will always be the same for TIFF files
    Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> frame;
    frame.resize(nrows, ncols);
    memcpy(&frame(0, 0), &_data[0], _length);
    return frame;
}
}
#endif