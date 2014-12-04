#include "HDF5Data.h"
#include "blosc.h"
#include "blosc_filter.h"

namespace SX
{

namespace Data
{

IData* HDF5Data::create(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer)
{
	return new HDF5Data(filename,diffractometer);
}

HDF5Data::HDF5Data(const std::string& filename, std::shared_ptr<Diffractometer> instrument)
:IData(filename,instrument)
{
	_metadata->add<std::string>("Instrument",instrument->getType());
}

HDF5Data::~HDF5Data()
{
	if (_isOpened)
		close();
}

void HDF5Data::open()
{
	if (_isOpened)
		return;

	try
	{
		_file=new H5::H5File(_filename.c_str(), H5F_ACC_RDONLY);
	}catch(...)
	{
		if (_file)
			delete _file;
		throw;
	}

	blosc_init();
	blosc_set_nthreads(4);

	// Register blosc filter dynamically with HDF5
	char *version, *date;
	int r= register_blosc(&version, &date);
	if (r<=0)
		throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

	// Create new data set
	try
	{
	_dataset=new H5::DataSet(_file->openDataSet("/counts"));
	// Dataspace of the dataset /counts
	_space=new H5::DataSpace(_dataset->getSpace());
	}catch(...)
	{
		throw;
	}
	// Get rank of data
	const hsize_t ndims=_space->getSimpleExtentNdims();
	hsize_t dims[ndims];
	hsize_t maxdims[ndims];
	// Get dimensions of data
	_space->getSimpleExtentDims(dims,maxdims);

	_nFrames=dims[0];
	_nrows=dims[1];
	_ncols=dims[2];

    // Size of one hyperslab
    hsize_t  count[3];
    count[0] = 1;
    count[1] = _nrows;
    count[2] = _ncols;

    _memspace=new H5::DataSpace(3,count,NULL);

	_isOpened=true;
}

void HDF5Data::close()
{
	_file->close();
	_space->close();
	_memspace->close();
	_dataset->close();
	delete _memspace;
	delete _space;
	delete _dataset;
	delete _file;

	blosc_destroy();

	_isOpened=false;
}

void HDF5Data::readInMemory()
{
	if (_inMemory)
		return;

	if (!_isOpened)
		open();

	_data.resize(_nFrames);
	for (std::size_t i=0;i<_nFrames;++i)
	{
		_data[i].resize(_nrows,_ncols);
	}

	hsize_t offset[3];
	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;

    // Size of one hyperslab
    hsize_t  count[3];
    count[0] = 1;
    count[1] = _nrows;
    count[2] = _ncols;

	// Read data Slab by slab
	for(offset[0]=0; offset[0] < _nFrames; offset[0] += count[0])
	{
		_space->selectHyperslab(H5S_SELECT_SET,count,offset,NULL,NULL);
		_dataset->read(_data.at(offset[0]).data(),H5::PredType::NATIVE_INT32,*_memspace,*_space);
	}

	_inMemory=true;

}

Eigen::MatrixXi HDF5Data::getFrame(std::size_t frame)
{
	if (_inMemory)
		return _data[frame];
	else
		return readFrame(frame);

}

Eigen::MatrixXi HDF5Data::readFrame(std::size_t frame)
{
	if (!_isOpened)
		open();

	Eigen::MatrixXi m(_nrows,_ncols);

	hsize_t count[3]={1,_nrows,_ncols};

	hsize_t offset[3]={frame,0,0};

	_space->selectHyperslab(H5S_SELECT_SET,count,offset,NULL,NULL);
	_dataset->read(m.data(),H5::PredType::NATIVE_INT32,*_memspace,*_space);

	return m;

}

} // Namespace Data

} // Namespace SX
