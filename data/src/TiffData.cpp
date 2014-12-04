#include "TiffData.h"

namespace SX
{

namespace Data
{


TiffData::TiffData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer, bool inMemory)
: IData(filename,diffractometer,inMemory)
 {
	uint32 w,h;

	_file=TIFFOpen(filename.c_str(),"r");

	if (!_file)
		throw std::runtime_error("Could not read "+filename+" as tif file");

	TIFFGetField(_file, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(_file, TIFFTAG_IMAGELENGTH, &h);

	if (w!=_ncols || h!=_nrows)
	{
		TIFFClose(_file);
		throw std::range_error("Tiff file "+filename+ " not consistent with diffractometer definition");
	}
	TIFFGetField(_file, TIFFTAG_BITSPERSAMPLE, &_bits);
	TIFFClose(_file);

	_nFrames=1;

	_data.reserve(_nFrames);

	_metadata->add<std::string>("Instrument",diffractometer->getType());

}

TiffData::~TiffData()
{
}

void TiffData::map()
{
	try
	{
		_file=TIFFOpen(_filename.c_str(),"r");
	}catch(...)
	{
		throw;
	}
}

void TiffData::unMap()
{
	TIFFClose(_file);
}

void TiffData::loadAllFrames()
{
	Eigen::Matrix<uint16,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> data16(_nrows,_ncols);

	// Read line per line
	for(unsigned short int i=0; i< _nrows; ++i)
		TIFFReadScanline(_file, (char*)&data16(i,0), i);

	_data[0].resize(_nrows,_ncols);

	// Not very nice, but need to copy the 16bits data to int
	_data[0]=data16.cast<int>();

	_inMemory=true;
}

Eigen::MatrixXi TiffData::getFrame(std::size_t idx)
{

	if (_inMemory)
		return _data[idx];
	else
		return readFrame(idx);
}

Eigen::MatrixXi TiffData::readFrame(std::size_t idx) const
{
	assert(idx<_nFrames);

	Eigen::Matrix<uint16,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> data16(_nrows,_ncols);
		// Read line per line
		for(unsigned short int i=0; i< _nrows; ++i)
			TIFFReadScanline(_file, (char*)&data16(i,0), i);
		// Not very nice, but need to copy the 16bits data to int
		return data16.cast<int>();
}

void TiffData::releaseMemory()
{
	_data[0].resize(0,0);
	_data.clear();
	_data.shrink_to_fit();
}

} // Namespace Data
} // Namespace SX
