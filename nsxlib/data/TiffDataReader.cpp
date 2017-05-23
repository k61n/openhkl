#include "../data/TiffDataReader.h"
#include "../instrument/ComponentState.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"

namespace nsx {

IDataReader* TiffDataReader::create(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer)
{
    return new TiffDataReader(filename,diffractometer);
}


TiffDataReader::TiffDataReader(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer)
: IDataReader(filename,diffractometer)
 {
    uint32 w,h;

    _file=TIFFOpen(filename.c_str(),"r");

    if (!_file)
        throw std::runtime_error("Could not read "+filename+" as tif file");

    TIFFGetField(_file, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(_file, TIFFTAG_IMAGELENGTH, &h);

    if (w!=_nCols || h!=_nRows)
    {
        close();
        throw std::range_error("Tiff file "+filename+ " not consistent with diffractometer definition");
    }
    _isOpened=false;
    TIFFGetField(_file, TIFFTAG_BITSPERSAMPLE, &_bits);

    if (_bits!=16 && _bits!=32)
    {
        close();
        throw std::runtime_error("Can't read TIFF file "+filename+" : only 16/32bits format supported");
    }

    _nFrames=1;

    _states.resize(_nFrames);
    _states[0].detector = ComponentState(_diffractometer->getDetector().get(), {});
    _states[0].sample = ComponentState(_diffractometer->getSample().get(), {});

    _metadata.add<std::string>("Instrument",diffractometer->getType());
}

void TiffDataReader::open()
{
    if (_isOpened)
        return;
    try
    {
        _file=TIFFOpen(_metadata.getKey<std::string>("filename").c_str(),"r");
    }catch(...)
    {
        throw;
    }
    _isOpened=true;
}

void TiffDataReader::close()
{
    if (_isOpened)
        TIFFClose(_file);
    _isOpened=false;
}


Eigen::MatrixXi TiffDataReader::getData(std::size_t frame)
{

    if (!_isOpened) {
        open();
    }

    if (_bits==16)
    {
        Eigen::Matrix<uint16,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> data16(_nRows,_nCols);
        // Read line per line
        for(unsigned short int i=0; i< _nRows; ++i)
            TIFFReadScanline(_file, (char*)&data16(i,0), i);
        // Not very nice, but need to copy the 16bits data to int
        return data16.cast<int>();
    }else
    {
        Eigen::Matrix<uint32,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> data32(_nRows,_nCols);
        // Read line per line
        for(unsigned short int i=0; i< _nRows; ++i)
            TIFFReadScanline(_file, (char*)&data32(i,0), i);
        // Not very nice, but need to copy the 32bits data to int
        return data32.cast<int>();
    }
}

} // end namespace nsx
