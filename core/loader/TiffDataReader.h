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

#include "core/loader/IDataReader.h" 
#include "core/loader/RawDataReader.h" 
#include "core/raw/DataKeys.h"

#include "tiffio.h"
#include <string>
#include <vector>

#include <filesystem>

namespace ohkl {
    struct TiffDataReaderParameters {
    std::string dataset_name = kw_datasetDefaultName;
    double wavelength = 0.0;
    double delta_omega = 0.0;
    double delta_chi = 0.0;
    double delta_phi = 0.0;
    //bool row_major = true;
    bool swap_endian = false;
    std::size_t bpp = 2;
    double baseline = 0.0;
    double gain = 1.0;
    // we use this to store the number of pixel which are rebinned
    // we always rebin symmetrically and thus just store the number of
    // total rebinned pixels 2x2 -> 4, 4x4 -> 16
    int data_binnnig;

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

    // struct to hold Tiff Tags for an opened Tiff file
    struct tif_file_metadata{
        uint32 _width = -1;
        uint32 _image_length = -1;
        uint16 _bits_per_pixel = -1;
        uint16 _compression = -1;
        uint16 _photometric = -1;
        uint16 _planar_config = -1;
        uint32 _npixels = -1;        

        friend std::ostream &operator <<(std::ostream &oss, const tif_file_metadata &o){ 
            oss
                << " * * * TIF FILE META DATA * * *\n" << std::setfill('.') 
                << "\nwidth:" << std::setw(20) << o._width
                << "\nheight:" << std::setw(19) << o._image_length
                << "\nbpp:" << std::setw(22) << o._bits_per_pixel
                << "\ncompression: " << std::setw(13) << o._compression
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
            void open() override {};
            //! Close Files
            void close() override {};
            //! initRead
            bool initRead() override;
            //! access data
            Eigen::MatrixXi data(size_t frame) override final;
            //! set Parameters
            void setParameters(const TiffDataReaderParameters& parameters);
            //! Swap Endians
            void swapEndian();
            //! Read Tags from Tiff file. Will throw error if necessary tag is not available
            void readTags(TIFF* );
            //! Read data from Tiff file
            void readData();
            //! Will rebin data
            void rebin(int rebins);
            //! Set Width
            void setWidth(int nwidth)  {_nwidth = nwidth;};
            //! Set Height
            void setHeight(int nheight) {_nheight = nheight;};
            //! Will scan file for tif meta data
            tif_file_metadata scanFile(std::string filename);
            //! will register file dimensions as meta data
            void registerFileDimension(std::string filename); 

            std::vector<std::string> readFileResolutions(std::vector<std::string> filenames);
           
        private:
            template <typename T_> Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> matrixFromData() const;
            
            std::vector<std::string> _filenames;
            std::vector<std::string> _file_resolutions;
            TiffDataReaderParameters _parameters; // need this too here ?
            std::size_t _length = 0;
            std::vector<char> _data;
            std::vector<uint16> _buffer; 

            tif_file_metadata _tif_meta_data;

            int _nwidth;
            int _nheight;

            TIFF* _tif;
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