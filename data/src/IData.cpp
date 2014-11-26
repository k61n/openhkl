#include <utility>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include<boost/filesystem/operations.hpp>

#include "IData.h"
#include "Detector.h"
#include "Gonio.h"
#include "Sample.h"
#include <hdf5.h>
#include "blosc_filter.h"

namespace SX
{

namespace Data
{

using namespace boost::filesystem;

IData::IData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer, bool inMemory)
: _filename(filename),
  _nFrames(0),
  _diffractometer(diffractometer),
  _metadata(new MetaData()),
  _inMemory(inMemory),
  _data(),
  _detectorStates(),
  _sampleStates(),
  _peaks(),
  _isMapped(false),
  _fileSize(0)
{
	if ( !boost::filesystem::exists(_filename.c_str()))
		throw std::runtime_error("ILLAsciiData, file: "+_filename+" does not exist");

}

IData::~IData()
{
	clearPeaks();
	delete _metadata;
}

std::string IData::getBasename() const
{
	path pathname(_filename);
	return pathname.filename().string();
}

const std::vector<Eigen::MatrixXi>& IData::getData() const
{
	return _data;
}

Eigen::MatrixXi& IData::getData(std::size_t idx)
{
	return _data[idx];
}

const std::string& IData::getFilename() const
{
	return _filename;
}

std::shared_ptr<Diffractometer> IData::getDiffractometer() const
{
	return _diffractometer;
}

MetaData* const IData::getMetadata() const
{
	return _metadata;
}

std::size_t IData::getNFrames() const
{
	return _nFrames;
}

std::set<Peak3D*>& IData::getPeaks()
{
	return _peaks;
}

void IData::addPeak(Peak3D* peak)
{
	_peaks.insert(peak);
}

void IData::clearPeaks()
{
	for (auto ptr : _peaks)
		delete ptr;
	_peaks.clear();
}

bool IData::isInMemory() const
{
	return (!_inMemory);
}

ComponentState IData::getDetectorInterpolatedState(double frame)
{

	if (frame>(_detectorStates.size()-1) || frame<0)
		throw std::runtime_error("Error when interpolating detector states: invalid frame value");

	std::size_t idx=static_cast<std::size_t>(std::floor(frame));

	std::size_t nPhysicalAxes=_diffractometer->getDetector()->getGonio()->getNPhysicalAxes();

	const std::vector<double> prevState=_detectorStates[idx].getValues();
	const std::vector<double> nextState=_detectorStates[idx+1].getValues();
	std::vector<double> state(nPhysicalAxes);
	for (std::size_t i=0;i<nPhysicalAxes;++i)
		state[i] = prevState[i] + (frame-static_cast<double>(idx))*(nextState[i]-prevState[i]);

	return _diffractometer->getDetector()->createState(state);

}

const ComponentState& IData::getDetectorState(int frame) const
{
	if (frame>(_detectorStates.size()-1) || frame<0)
			throw std::runtime_error("Error when returning detector state: invalid frame value");
	return _detectorStates[frame];
}

ComponentState IData::getSampleInterpolatedState(double frame)
{

	if (frame>(_sampleStates.size()-1) || frame<0)
		throw std::runtime_error("Error when interpolating sample states: invalid frame value");

	std::size_t idx=static_cast<std::size_t>(std::floor(frame));

	std::size_t nPhysicalAxes=_diffractometer->getSample()->getGonio()->getNPhysicalAxes();

	const std::vector<double> prevState=_sampleStates[idx].getValues();
	const std::vector<double> nextState=_sampleStates[idx+1].getValues();
	std::vector<double> state(nPhysicalAxes);
	for (std::size_t i=0;i<nPhysicalAxes;++i)
		state[i] = prevState[i] + (frame-static_cast<double>(idx))*(nextState[i]-prevState[i]);

	return _diffractometer->getSample()->createState(state);

}

const ComponentState& IData::getSampleState(int frame) const
{
	if (frame>(_sampleStates.size()-1) || frame<0)
			throw std::runtime_error("Error when returning sample state: invalid frame value");
	return _sampleStates[frame];
}

const std::vector<ComponentState>& IData::getDetectorStates() const
{
	return _detectorStates;
}

const std::vector<ComponentState>& IData::getSampleStates() const
{
	return _sampleStates;
}

bool IData::removePeak(Peak3D* peak)
{
	auto it=_peaks.find(peak);
	if (it==_peaks.end())
		return false;

	delete *it;
    _peaks.erase(it);
	return true;
}

bool IData::isMapped() const
{
	return _isMapped;
}

std::size_t IData::getFileSize() const
{
	return _fileSize;
}

void IData::saveHDF5(const std::string& filename)
{
	if (!_inMemory)
		throw std::runtime_error("Can't save "+_filename+" as HDF5, file not in memory");

	int nrows=_diffractometer->getDetector()->getNRows();
	int ncols=_diffractometer->getDetector()->getNCols();

	hsize_t dims[3] = {_nFrames, ncols,nrows};
	hsize_t  chunk[3] = {1, ncols,nrows};
	hsize_t  count[3] = {1, ncols,nrows};

	std::string fullName=filename+".h5";
	hid_t file_id = H5Fcreate(fullName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	/// DATASPACE IS CHUNKED ///
	hid_t space = H5Screate_simple (3, dims, NULL);
	hid_t dcpl = H5Pcreate (H5P_DATASET_CREATE);
	hid_t status = H5Pset_chunk(dcpl, 3, chunk);

	char *version, *date;
	int r;
	unsigned int cd_values[7];
	/* But you can also taylor Blosc parameters to your needs */
	/* 0 to 3 (inclusive) param slots are reserved. */
	cd_values[4] = 9;       /* compression level */
	cd_values[5] = 1;       /* 0: shuffle not active, 1: shuffle active */
	cd_values[6] = BLOSC_LZ4; /* the actual compressor to use */

	r = register_blosc(&version, &date);
	std::cout << version << "," << date << std::endl;
	//if (r>0)
		r = H5Pset_filter(dcpl, FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

	  hid_t HDF5_datatype;
	  HDF5_datatype = H5T_NATIVE_UINT32;
	  hid_t  memspace = H5Screate_simple(3, count, NULL);
	  hid_t dataset_id = H5Dcreate2(file_id, "data", HDF5_datatype, space, H5P_DEFAULT, dcpl, H5P_DEFAULT);

	  hsize_t offset[3];
	  offset[0] = 0;
	  offset[1] = 0;
	  offset[2] = 0;

	  std::cout << "Writting data \n";
	  for(offset[0]=0; offset[0]<_data.size(); offset[0] += count[0])
		{

		  herr_t errstatus = H5Sselect_hyperslab(space, H5S_SELECT_SET, offset, NULL, count, NULL);
		  status = H5Dwrite(dataset_id, HDF5_datatype, memspace, space, H5P_DEFAULT,  _data.at(offset[0]).data());
		  if(status <0)
			throw std::runtime_error("Problem writting hdf5 file");
		}

	  H5Sclose(memspace);
	  H5Sclose(space);
	  H5Dclose(dataset_id);
	  H5Fclose(file_id);
	  H5Pclose(dcpl);
}

void IData::readHDF5(const std::string& filename)
{
	std::cout << "Reading data" <<std::endl;


	int nrows=_diffractometer->getDetector()->getNRows();
	int ncols=_diffractometer->getDetector()->getNCols();

	hid_t dcpl = H5Pcreate (H5P_DATASET_CREATE);
	hsize_t  chunk[3] = {1, ncols,nrows};
	hid_t status = H5Pset_chunk(dcpl, 3, chunk);

	char *version, *date;
	int r;
	unsigned int cd_values[7];
	/* But you can also taylor Blosc parameters to your needs */
	/* 0 to 3 (inclusive) param slots are reserved. */
	cd_values[4] = 9;       /* compression level */
	cd_values[5] = 1;       /* 0: shuffle not active, 1: shuffle active */
	cd_values[6] = BLOSC_LZ4; /* the actual compressor to use */

	r = register_blosc(&version, &date);
	std::cout << version << "," << date << std::endl;
	//if (r>0)
	r = H5Pset_filter(dcpl, FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

	_data.resize(_nFrames);
	for (int i=0;i<_nFrames;++i)
	{
		_data[i].resize(nrows,ncols);
	}

	hid_t HDF5_datatype;
    HDF5_datatype = H5T_NATIVE_UINT32;

	hid_t file_id = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	std::cout << "file_id" << file_id << std::endl;
	hid_t dataset_id = H5Dopen1(file_id, "/data");
	std::cout << "dataset_id" << dataset_id << std::endl;
	hid_t space_id = H5Dget_space(dataset_id); //filespace id
	std::cout << "space_id" << space_id << std::endl;
	const int ndims = H5Sget_simple_extent_ndims(space_id);
	hsize_t dims[ndims];
	hsize_t maxdims[ndims];
	H5Sget_simple_extent_dims(space_id, dims, maxdims);
	hsize_t  count[3];
	count[0] = 1; // hyperslab size is one image
	count[1] = dims[1]; // DIMX * DIMY
	count[2] = dims[2]; // DIMX * DIMY
//
	hid_t memspace_id =  H5Screate_simple(3, count, NULL); // corresponds to the hyperslab
	std::cout << "memspace_id" << memspace_id << std::endl;
	hsize_t offset[3];
	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;

	//
	for(offset[0]=0; offset[0] < _nFrames; offset[0] += count[0])
	{
	  herr_t errstatus = H5Sselect_hyperslab(space_id, H5S_SELECT_SET, offset, NULL, count, NULL);
	  hid_t status = H5Dread(dataset_id, HDF5_datatype, memspace_id, space_id , H5P_DEFAULT,  _data.at(offset[0]).data());
	}
	H5Dclose(dataset_id);
	H5Fclose(file_id);
}

} // end namespace Data

} // end namespace SX
