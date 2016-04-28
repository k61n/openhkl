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
:IData(filename,instrument), _dataset(nullptr), _space(nullptr), _memspace(nullptr)
{
    _isCached = false;
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

	std::vector<std::string> axesS=_diffractometer->getDetector()->getGonio()->getPhysicalAxesNames();

	Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> dm(axesS.size(),_nFrames);
	for (unsigned int i=0;i<axesS.size();++i)
	{
		try
		{
			H5::DataSet dset=detectorGroup.openDataSet(axesS[i]);
			H5::DataSpace space(dset.getSpace());
			hsize_t dim=space.getSimpleExtentNdims();
			if (dim!=1)
				throw std::runtime_error("Read HDF5, problem reading detector scan parameters, dimension of array should be 1");;
			hsize_t dims[dim], maxdims[dim];
			space.getSimpleExtentDims(dims,maxdims);
			if (dims[0]!=_nFrames)
				throw std::runtime_error("Read HDF5, problem reading detector scan parameters, different array length to npdone");
			dset.read(&dm(i,0),H5::PredType::NATIVE_DOUBLE,space,space);
		}catch(...)
		{
			throw;
		}
	}

	// Use natural units internally (rad)
	dm*=SX::Units::deg;

	_detectorStates.reserve(_nFrames);
	for (unsigned int i=0;i<_nFrames;++i)
	{
		_detectorStates.push_back(_diffractometer->getDetector()->createStateFromEigen(dm.col(i)));
	}

	// Getting Scan parameters for the sample
	H5::Group sampleGroup(_file->openGroup("/Data/Scan/Sample"));

	axesS=_diffractometer->getSample()->getGonio()->getPhysicalAxesNames();

	dm.resize(axesS.size(),_nFrames);
	for (unsigned int i=0;i<axesS.size();++i)
	{
		try
		{
			H5::DataSet dset=sampleGroup.openDataSet(axesS[i]);
			H5::DataSpace space(dset.getSpace());
			hsize_t dim=space.getSimpleExtentNdims();
			if (dim!=1)
				throw std::runtime_error("Read HDF5, problem reading sample scan parameters, dimension of array should be 1");;
			hsize_t dims[dim], maxdims[dim];
			space.getSimpleExtentDims(dims,maxdims);
			if (dims[0]!=_nFrames)
				throw std::runtime_error("Read HDF5, problem reading sample scan parameters, different array length to npdone");
			dset.read(&dm(i,0),H5::PredType::NATIVE_DOUBLE,space,space);
		}catch(...)
		{
			throw;
		}
	}

	// Use natural units internally (rad)
	dm*=SX::Units::deg;

	_sampleStates.reserve(_nFrames);
	for (unsigned int i=0;i<_nFrames;++i)
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
	if (!_isOpened)
		return;

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


Eigen::MatrixXi HDF5Data::readFrame(std::size_t frame)
{
	if (!_isOpened)
		open();

    // HDF5 specification requires row-major storage
	Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> m(_nrows,_ncols);

	hsize_t count[3]={1,_nrows,_ncols};
	hsize_t offset[3]={frame,0,0};

	_space->selectHyperslab(H5S_SELECT_SET,count,offset,NULL,NULL);
	_dataset->read(m.data(),H5::PredType::NATIVE_INT32,*_memspace,*_space);

    // return copy as MatrixXi (which could be col-major)
	return Eigen::MatrixXi(m);

}

} // Namespace Data

} // Namespace SX
