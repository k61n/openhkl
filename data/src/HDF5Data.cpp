#include "HDF5Data.h"
#include "blosc.h"
#include "blosc_filter.h"
#include "Detector.h"
#include "Gonio.h"
#include "Sample.h"
#include "Units.h"

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
	_file=new H5::H5File(_filename.c_str(), H5F_ACC_RDONLY);

	// Read the info group and store in metadata
	H5::Group infoGroup(_file->openGroup("/Info"));
	int ninfo=infoGroup.getNumAttrs();
	for (int i=0;i<ninfo;++i)
	{
		H5::Attribute attr=infoGroup.openAttribute(i);
		H5::DataType typ=attr.getDataType();
		std::string value;
		attr.read(typ,value);
		_metadata->add<std::string>(attr.getName(),value);
	}

	// Read the experiment group and store all int and double attributes in metadata
	H5::Group experimentGroup(_file->openGroup("/Experiment"));
	int nexps=experimentGroup.getNumAttrs();
	for (int i=0;i<nexps;++i)
	{
		H5::Attribute attr=experimentGroup.openAttribute(i);
		H5::DataType typ=attr.getDataType();
		if (typ==H5::PredType::NATIVE_INT32)
		{
			int value;
			attr.read(typ,&value);
			_metadata->add<int>(attr.getName(),value);
		}
		if (typ==H5::PredType::NATIVE_DOUBLE)
		{
			double value;
			attr.read(typ,&value);
			_metadata->add<double>(attr.getName(),value);
		}
	}

	_nFrames=_metadata->getKey<int>("npdone");


	// Getting Scan parameters for the detector
	H5::Group detectorGroup(_file->openGroup("/Data/Scan/Detector"));
	int nAxes=detectorGroup.getNumAttrs();

	if (nAxes!=_diffractometer->getDetector()->getNPhysicalAxes())
	{
		_file->close();
		throw std::runtime_error("Number of Detector Axes in HDF5 definition different from Diffractometer definition");
	}
	std::vector<std::string> axesS=_diffractometer->getDetector()->getGonio()->getPhysicalAxesNames();

	Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> dm(nAxes,_nFrames);
	for (int i=0;i<nAxes;++i)
	{
		H5::Attribute attr=detectorGroup.openAttribute(axesS[i]);
		attr.read(H5::PredType::NATIVE_DOUBLE,&dm(i,0));
	}

	// Use natural units internally (rad)
	dm*=SX::Units::deg;

	_detectorStates.reserve(_nFrames);
	for (int i=0;i<_nFrames;++i)
	{
		_detectorStates.push_back(_diffractometer->getDetector()->createStateFromEigen(dm.col(i)));
	}

	// Getting Scan parameters for the sample
	H5::Group sampleGroup(_file->openGroup("/Data/Scan/Sample"));
	nAxes=sampleGroup.getNumAttrs();

	if (nAxes!=_diffractometer->getSample()->getNPhysicalAxes())
	{
		_file->close();
		throw std::runtime_error("Number of Sample Axes in HDF5 definition different from Diffractometer definition");
	}
	axesS=_diffractometer->getSample()->getGonio()->getPhysicalAxesNames();

	dm.resize(nAxes,_nFrames);
	for (int i=0;i<nAxes;++i)
	{
		H5::Attribute attr=sampleGroup.openAttribute(axesS[i]);
		attr.read(H5::PredType::NATIVE_DOUBLE,&dm(i,0));
	}

	// Use natural units internally (rad)
	dm*=SX::Units::deg;

	_sampleStates.reserve(_nFrames);
	for (int i=0;i<_nFrames;++i)
	{
		_sampleStates.push_back(_diffractometer->getSample()->createStateFromEigen(dm.col(i)));
	}

	_file->close();
	delete _file;
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
	_dataset=new H5::DataSet(_file->openDataSet("/Data/Counts"));
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
