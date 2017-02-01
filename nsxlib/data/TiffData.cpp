#include "../instrument/Detector.h"
#include "../instrument/Sample.h"
#include "TiffData.h"

namespace SX
{

namespace Data
{

IData* TiffData::create(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer)
{
    return new TiffData(filename,diffractometer);
}


TiffData::TiffData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer)
: IData(filename,diffractometer)
 {
    uint32 w,h;

    _file=TIFFOpen(filename.c_str(),"r");

    if (!_file)
        throw std::runtime_error("Could not read "+filename+" as tif file");

    TIFFGetField(_file, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(_file, TIFFTAG_IMAGELENGTH, &h);

    if (w!=_ncols || h!=_nrows)
    {
        close();
        throw std::range_error("Tiff file "+filename+ " not consistent with diffractometer definition");
    }
    _isOpened=false;
    TIFFGetField(_file, TIFFTAG_BITSPERSAMPLE, &_bits);

    if (_bits!=16 && _bits!=32)
    {
        close();
        throw std::runtime_error("Can't read TIFF file "+_filename+" : only 16/32bits format supported");
    }

    _nFrames=1;

    _data.reserve(_nFrames);

    _detectorStates.reserve(_nFrames);
    _detectorStates.push_back(_diffractometer->getDetector()->createState());
    _sampleStates.reserve(_nFrames);
    _sampleStates.push_back(_diffractometer->getSample()->createState());

    _metadata->add<std::string>("Instrument",diffractometer->getType());

}

TiffData::~TiffData()
{
}

void TiffData::open()
{
    if (_isOpened)
        return;
    try
    {
        _file=TIFFOpen(_filename.c_str(),"r");
    }catch(...)
    {
        throw;
    }
    _isOpened=true;
}

void TiffData::close()
{
    if (_isOpened)
        TIFFClose(_file);
    _isOpened=false;
}


Eigen::MatrixXi TiffData::readFrame(std::size_t idx)
{

    if (!_isOpened)
        open();

    if (_bits==16)
    {
        Eigen::Matrix<uint16,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> data16(_nrows,_ncols);
        // Read line per line
        for(unsigned short int i=0; i< _nrows; ++i)
            TIFFReadScanline(_file, (char*)&data16(i,0), i);
        // Not very nice, but need to copy the 16bits data to int
        return data16.cast<int>();
    }else
    {
        Eigen::Matrix<uint32,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> data32(_nrows,_ncols);
        // Read line per line
        for(unsigned short int i=0; i< _nrows; ++i)
            TIFFReadScanline(_file, (char*)&data32(i,0), i);
        // Not very nice, but need to copy the 32bits data to int
        return data32.cast<int>();
    }
}

} // Namespace Data
} // Namespace SX
