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

#include "TiffDataReader.h"

#include "base/parser/EigenToVector.h"
#include "base/parser/Parser.h"
#include "base/utils/StringIO.h" // join
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/gonio/Component.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/raw/DataKeys.h"

namespace ohkl {
    TiffDataReader::TiffDataReader() : 
    IDataReader("::NO-FILENAME::"), _tif(nullptr)
    {} 

    TiffDataReader::~TiffDataReader()
    {}

    // only used when we need to read file resolutions of fiff files before creating DataSet
    std::vector<std::string> TiffDataReader::readFileResolutions(std::vector<std::string> filenames)
    {
        std::vector<std::string> res;
        for (auto & fname : filenames) {
            auto tags = scanFile(fname);
            res.emplace_back(
                std::to_string(tags._width) + "x" + std::to_string(tags._image_length)
            );
        }
        return res;
    }

    // to use with readFileResolutions before having a DataSet
    tif_file_metadata TiffDataReader::scanFile(std::string filename)
    {
        tif_file_metadata tags;

        if (!filename.empty()) {
            auto tif = TIFFOpen(filename.c_str(), "r");
            if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &tags._width) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t TIFFTAG_IMAGEWIDTH is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &tags._image_length) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t TIFFTAG_IMAGELENGTH is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &tags._bits_per_pixel) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t TIFFTAG_BITSPERSAMPLE is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_COMPRESSION, &tags._compression) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t  TIFFTAG_COMPRESSION is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &tags._planar_config) == 0) // how data is saved (1) means RGBRGBRGB...RGB
               throw std::runtime_error("E\t  TiffDataReader::read\t  TIFFTAG_PLANARCONFIG is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &tags._photometric) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t  TIFFTAG_PHOTOMETRIC is not defined in *.tiff file");
            TIFFClose(tif);
        }
        return tags;
    }

    void TiffDataReader::readTags(TIFF* tif)
    {
        if (tif){
            if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &_tif_meta_data._width) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t TIFFTAG_IMAGEWIDTH is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &_tif_meta_data._image_length) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t TIFFTAG_IMAGELENGTH is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &_tif_meta_data._bits_per_pixel) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t TIFFTAG_BITSPERSAMPLE is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_COMPRESSION, &_tif_meta_data._compression) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t  TIFFTAG_COMPRESSION is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &_tif_meta_data._planar_config) == 0) // how data is saved (1) means RGBRGBRGB...RGB
               throw std::runtime_error("E\t  TiffDataReader::read\t  TIFFTAG_PLANARCONFIG is not defined in *.tiff file");
            if (TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &_tif_meta_data._photometric) == 0)
                throw std::runtime_error("E\t  TiffDataReader::read\t  TIFFTAG_PHOTOMETRIC is not defined in *.tiff file");
        } else
            throw std::runtime_error("E\t  TiffDataReader::readTags\t No file has been opened");
    }

    void TiffDataReader::readData()
    {
        if (_tif)
        {
            _tif_meta_data._npixels = _tif_meta_data._image_length * _tif_meta_data._width;

            auto size = TIFFScanlineSize(_tif);

            // Explanation: data works as our buffer to hold all read scanlines from the tiff file
            // that is before rebinning. Then we are using this again for storing the rebinned data
            // this is possible because indexes do not overlap with eahc other
            _buffer.resize(_tif_meta_data._npixels);
            _data.resize(4*_tif_meta_data._npixels);

            // read all single lines from tiff file and store them in data
            for (uint32 row = 0; row <  _tif_meta_data._image_length; row++)
		        TIFFReadScanline(_tif, (tdata_t*)(_buffer.data()+row*size/2), row);

            // rebinning
            rebin(_parameters.data_binnnig);

            //std::memcpy(_data.data(), _buffer.data(), _buffer.size()/16); // why does this already work was 2 before
            std::memcpy(_data.data(), _buffer.data(),  2*_buffer.size()/_parameters.data_binnnig);

            // updating dimensions
            _length = 2*_buffer.size()/_parameters.data_binnnig;
        } else
            throw std::runtime_error("E TiffDataReader::readData No stream has been opened"); 
    }

    void TiffDataReader::rebin(int rebins)
    {
        if (rebins == 1 ) return;

        int sqrt_rbins =  sqrt(rebins);
        //
        // rebinning of data
        int nidx; // new index (after rebinning)
        std::vector<int> oidx; // old indices (before rebinning) -> we are rebinning oidx.size() to one new index
        oidx.resize(rebins);
        double tmp;

        for (int j = 0; j<_tif_meta_data._image_length-(sqrt_rbins); j+=sqrt_rbins)
            for(int i = 0; i<_tif_meta_data._width-(sqrt_rbins);i+=sqrt_rbins){
                for (int k=0; k<rebins;k++){
                    oidx[k] = (k%sqrt_rbins) + i + (j+(k/sqrt_rbins))*_tif_meta_data._width;
                }

                nidx = (int)(i/sqrt_rbins)+(int)(j/sqrt_rbins)*(_tif_meta_data._width/sqrt_rbins);

                // simple averaging here;
                tmp = 0;
                for (auto & e : oidx)
                    tmp += 1.0 / rebins *  (double)_buffer.at(e);

                // We are storing the rebinned data in the same container as unbined
                // because the old and new indexes are not overlapping we can use one and the same container to 
                // store data of both
                _buffer[nidx] = (uint16)tmp;
            }
    }

    void TiffDataReader::registerFileDimension(std::string filename)
    {
        auto tags = scanFile(filename);
        _file_resolutions.emplace_back(std::to_string(tags._width) + " x " + std::to_string(tags._image_length));
    }

    bool TiffDataReader::initRead()
    {
        const bool init_success = IDataReader::initRead();
        if (!init_success)
            throw std::runtime_error("TiffDataReader::initRead(): initialisation failed");

        isInitialized = true;
        return isInitialized;
    }

    // for compatability with the rest of ohkl needed
    void TiffDataReader::addFrame(const std::string& filename)
    {
        checkInit();

        _filenames.push_back(filename);

        // Update sources list
        _dataset_out->metadata().add<std::string>(ohkl::at_datasetSources, ohkl::join(_filenames, ", "));
        // we read and store file dimensions
        registerFileDimension(filename);
        _dataset_out->metadata().add<std::string>(ohkl::at_datasetDimensions, ohkl::join(_file_resolutions, ","));

        const std::size_t nframes = _filenames.size();
        _dataset_out->metadata().add<int>(ohkl::at_frameCount, nframes);
        _dataset_out->datashape[2] = nframes;

        const auto& detector_gonio = _dataset_out->diffractometer()->detector()->gonio();
        size_t n_detector_gonio_axes = detector_gonio.nAxes();

        const auto& sample_gonio = _dataset_out->diffractometer()->sample().gonio();
        size_t n_sample_gonio_axes = sample_gonio.nAxes();

        int omega_idx = -1, phi_idx = -1, chi_idx = -1;
        for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
            const std::string axis_name = sample_gonio.axis(i).name();
            omega_idx = axis_name == ohkl::ax_omega ? int(i) : omega_idx;
            chi_idx = axis_name == ohkl::ax_chi ? int(i) : chi_idx;
            phi_idx = axis_name == ohkl::ax_phi ? int(i) : phi_idx;
        }

        if (omega_idx == -1 || phi_idx == -1 || chi_idx == -1)
            throw std::runtime_error("TiffDataReader: could not find angle indices");

        const std::size_t idx = nframes - 1;

        std::vector<double> det_states(n_detector_gonio_axes);
        std::fill(det_states.begin(), det_states.end(), 0.0);
        _dataset_out->diffractometer()->detectorStates.emplace_back(std::move(det_states));

        std::vector<double> sample_states(n_sample_gonio_axes);
        std::fill(sample_states.begin(), sample_states.end(), 0.0);
        sample_states[omega_idx] = idx * _parameters.delta_omega * deg;
        sample_states[phi_idx] = idx * _parameters.delta_phi * deg;
        sample_states[chi_idx] = idx * _parameters.delta_chi * deg; 

        _dataset_out->diffractometer()->sampleStates.emplace_back(std::move(sample_states));

    }

    Eigen::MatrixXi TiffDataReader::data(size_t frame)
    {
        checkInit();

        _tif = TIFFOpen(_filenames.at(frame).c_str(), "r");
        readTags(_tif);

         // Check if we can process this stream
        if (_tif_meta_data._compression != 1) // no compressed tiff files
            throw std::runtime_error("E\t  TiffDataReader::read\t  Compressed Tiff files are not supported");
        if (_tif_meta_data._planar_config != PLANARCONFIG_CONTIG) // need planar based tiff files
            throw std::runtime_error("E\t  TiffDataReader::read\t  Only PLANARCONFIG_CONTIG tiff files are supported");
        if (_tif_meta_data._photometric > 1) // tiff files can only have gray colors
            throw std::runtime_error("E\t  TiffDataReader::read\t  Colored Tiff files are not supported");

        _parameters.data_binnnig = (_tif_meta_data._width / _nwidth) * (_tif_meta_data._width / _nwidth);
        _parameters.bpp = _tif_meta_data._bits_per_pixel/8;

        readData();

        TIFFClose(_tif);

        if (_parameters.swap_endian)
        swapEndian();

        switch (_parameters.bpp) {
            case 1: return matrixFromData<uint8_t>().cast<int>();
            case 2: return matrixFromData<uint16_t>().cast<int>();
            case 3: return matrixFromData<uint32_t>().cast<int>();
            default: throw std::runtime_error("bpp unsupported: " + std::to_string(_parameters.bpp));
        }
        return Eigen::MatrixXi();
    }


void TiffDataReader::setParameters(const TiffDataReaderParameters& parameters)
{
    _parameters = parameters;

    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    _length = _parameters.bpp * nrows * ncols;
    auto& mono = _dataset_out->diffractometer()->source().selectedMonochromator();
    mono.setWavelength(_parameters.wavelength);

    _dataset_out->metadata().add<std::string>(
        ohkl::at_diffractometer, _dataset_out->diffractometer()->name());
    _dataset_out->metadata().add<double>(ohkl::at_wavelength, _parameters.wavelength);
    _dataset_out->metadata().add<double>(ohkl::at_monitorSum, 0.0);
    _dataset_out->metadata().add<int>(ohkl::at_numor, 0.0);
    _dataset_out->metadata().add<double>(ohkl::at_baseline, _parameters.baseline);
    _dataset_out->metadata().add<double>(ohkl::at_gain, _parameters.gain);
    _data.resize(_parameters.bpp * nrows * ncols);
}

void TiffDataReader::swapEndian()
{
    const std::size_t nrows = _dataset_out->nRows(), ncols = _dataset_out->nCols();

    for (std::size_t i = 0; i < nrows * ncols; ++i) {
        for (std::size_t byte = 0; byte < _parameters.bpp / 2; ++byte) {
            std::swap(
                _data[_parameters.bpp * i + byte],
                _data[_parameters.bpp * i + (_parameters.bpp - 1 - byte)]);
        }
    }
}
} // ohkl