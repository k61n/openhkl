#include <utility>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include "IData.h"
#include "Detector.h"
#include "Gonio.h"
#include "Sample.h"

#include "H5Cpp.h"
#include "blosc_filter.h"
#include "blosc.h"

namespace SX
{

namespace Data
{

using namespace boost::filesystem;

IData::IData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer, bool inMemory)
: _filename(filename),
  _nFrames(0),
  _nrows(0),
  _ncols(0),
  _diffractometer(diffractometer),
  _metadata(new MetaData()),
  _inMemory(inMemory),
  _data(),
  _detectorStates(),
  _sampleStates(),
  _peaks(),
  _isMapped(false),
  _fileSize(0),
  _masks()
{
	if ( !boost::filesystem::exists(_filename.c_str()))
		throw std::runtime_error("IData, file: "+_filename+" does not exist");

	_nrows=_diffractometer->getDetector()->getNRows();
	_ncols=_diffractometer->getDetector()->getNCols();
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

int IData::dataAt(int x, int y, int z)
{
    if (z<0 || z>=_nFrames || y<0 || y>=_nrows || x<0 || x>=_ncols)
        return 0;

    return (_data[z])(y,x);
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

	blosc_init();
	blosc_set_nthreads(4);

	if (!_inMemory)
	throw std::runtime_error("Can't save "+_filename+" as HDF5, file not in memory");

	hsize_t dims[3] = {_nFrames, _nrows,_ncols};
	hsize_t chunk[3] = {1, _nrows,_ncols};
	hsize_t count[3] = {1, _nrows,_ncols};

	H5::H5File file(filename.c_str(), H5F_ACC_TRUNC);

	H5::DataSpace* space=new H5::DataSpace(3,dims,NULL);

	H5::DSetCreatPropList* plist = new  H5::DSetCreatPropList();

	plist->setChunk(3,chunk);

	char *version, *date;
	int r;
	unsigned int cd_values[7];
	cd_values[4] = 9;       // Highest compression level
	cd_values[5] = 1;       // Bit shuffling active
	cd_values[6] = BLOSC_LZ4; // Seem to be the best compromise speed/compression for diffraction data

	r = register_blosc(&version, &date);
	if (r<=0)
		throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

	plist->setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

	H5::DataSpace* memspace=new H5::DataSpace(3,count,NULL);

	H5::DataSet* dset=new H5::DataSet(file.createDataSet("counts", H5::PredType::NATIVE_INT32, *space, *plist));

	hsize_t offset[3];
	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;

	for(offset[0]=0; offset[0]<_data.size(); offset[0] += count[0])
	{
	  space->selectHyperslab(H5S_SELECT_SET,count,offset,NULL,NULL);
	  dset->write(_data.at(offset[0]).data(),H5::PredType::NATIVE_INT32,*memspace,*space);
	}


	delete dset;
	delete memspace;
	delete space;
	delete plist;
	file.close();

	//
	blosc_destroy();
}

void IData::readHDF5(const std::string& filename)
{
	// Needed
	blosc_init();
	blosc_set_nthreads(4);

	char *version, *date;
	int r= register_blosc(&version, &date);

	if (r<=0)
		throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

	blosc_init();
	blosc_set_nthreads(8);

	H5::H5File file(filename.c_str(), H5F_ACC_RDONLY);

    H5::DataSet* dset=new H5::DataSet(file.openDataSet("/counts"));

	H5::DataSpace* space=new H5::DataSpace(dset->getSpace());

	// Get rank of data
	const hsize_t ndims=space->getSimpleExtentNdims();
	hsize_t dims[ndims];
	hsize_t maxdims[ndims];
	// Get dimensions
	space->getSimpleExtentDims(dims,maxdims);

	if (dims[0]!=_nFrames || dims[1]!=_nrows || dims[2]!=_ncols)
		throw std::range_error("IData: Data dimensions in HDF5 different from previsouly defined");

	_data.resize(_nFrames);
	for (std::size_t i=0;i<_nFrames;++i)
	{
		_data[i].resize(_nrows,_ncols);
	}

	// Size of one hyperslab
	hsize_t  count[3];
	count[0] = 1;
	count[1] = dims[1];
	count[2] = dims[2];

	H5::DataSpace* memspace=new H5::DataSpace(3,count,NULL);

	hsize_t offset[3];
	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;

	// Read data Slab by slab
	for(offset[0]=0; offset[0] < _nFrames; offset[0] += count[0])
	{
	  space->selectHyperslab(H5S_SELECT_SET,count,offset,NULL,NULL);
	  dset->read(_data.at(offset[0]).data(),H5::PredType::NATIVE_INT32,*memspace,*space);
	}

	delete memspace;
	delete space;
	delete dset;
	file.close();

	blosc_destroy();

}

void IData::addMask(AABB<double,3>* mask)
{
	// Insert the mask
	_masks.insert(mask);
	// Update the peaks with this mask
	maskPeaks(*mask);
}

void IData::removeMask(AABB<double,3>* mask)
{
	auto p=_masks.find(mask);
	if (p!=_masks.end())
		_masks.erase(mask);
	// Update the peaks selection status.
	unmaskPeaks();
}

void IData::maskPeaks() const
{
	for (auto p : _peaks)
	{
		// If the peak is already unselected, no need to mask it.
		if (!p->isSelected())
			continue;
		for (auto m : _masks)
		{
			// If the background of the peak intercept the mask, unselected the peak
			if (m->intercept(*(p->getBackground())))
			{
				p->setSelected(false);
				break;
			}
		}
	}
}

void IData::unmaskPeaks() const
{
	for (auto p : _peaks)
	{
		// If the peak is already selected, no need to unmask it.
		if (p->isSelected())
			continue;
		for (auto m : _masks)
		{
			// If the background of the peak intercept the mask, unselected the peak
			if (m->intercept(*(p->getBackground())))
			{
				p->setSelected(true);
				break;
			}
		}
	}
}

void IData::maskPeaks(const AABB<double,3>& mask) const
{
	// Loop over the peaks and check for each of them if it intercepts the mask
	for (auto p : _peaks)
	{
		// If the peak is already unselected, no need to mask it.
		if (!p->isSelected())
			continue;
		// If the background of the peak intercept the mask, unselected the peak
		if (mask.intercept(*(p->getBackground())))
		{
			p->setSelected(false);
			break;
		}
	}
}

bool IData::isMasked(const Eigen::Vector3d& point) const
{

	// Loop over the defined masks and return true if one of them contains the point
	for (auto m : _masks)
	{
		if (m->isInsideAABB(point))
			return true;
	}
	// No mask contains the point, return false
	return false;

}

} // end namespace Data

} // end namespace SX
