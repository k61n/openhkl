#include <utility>
#include <stdexcept>
#include "Units.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include "IData.h"
#include "Detector.h"
#include "Gonio.h"
#include "Sample.h"
#include "Source.h"

#include "H5Cpp.h"
#include "blosc_filter.h"
#include "blosc.h"
#include "Ellipsoid.h"

namespace SX
{

namespace Data
{

using namespace boost::filesystem;

IData::IData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer)
:  _isOpened(false),
   _filename(filename),
  _nFrames(0),
  _nrows(0),
  _ncols(0),
  _diffractometer(diffractometer),
  _metadata(new MetaData()),
  _inMemory(false),
  _data(),
  _detectorStates(),
  _sampleStates(),
  _peaks(),
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

const Eigen::MatrixXi& IData::getData(std::size_t idx) const
{
	return _data[idx];
}

int IData::dataAt(unsigned int x, unsigned int y, unsigned int z)
{

	// Check that the voxel is inside the limit of the data
    if (z<0 || z>=_nFrames || y<0 || y>=_ncols || x<0 || x>=_nrows)
        return 0;

    return (_data[z])(x,y);
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

std::size_t IData::getNCols() const
{
	return _ncols;
}

std::size_t IData::getNRows() const
{
	return _nrows;
}

std::set<Peak3D*>& IData::getPeaks()
{
	return _peaks;
}

void IData::addPeak(Peak3D* peak)
{
	_peaks.insert(peak);
	maskPeak(peak);
}

void IData::clearPeaks()
{
	for (auto ptr : _peaks)
		delete ptr;
	_peaks.clear();
}

bool IData::isInMemory() const
{
	return (_inMemory);
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

const ComponentState& IData::getDetectorState(unsigned int frame) const
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

const ComponentState& IData::getSampleState(unsigned int frame) const
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

bool IData::isOpened() const
{
	return _isOpened;
}

std::size_t IData::getFileSize() const
{
	return _fileSize;
}

void IData::saveHDF5(const std::string& filename) const
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
	cd_values[6] = BLOSC_BLOSCLZ; // Seem to be the best compromise speed/compression for diffraction data

	r = register_blosc(&version, &date);
	if (r<=0)
		throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

	plist->setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

	H5::DataSpace* memspace=new H5::DataSpace(3,count,NULL);

	H5::Group* dataGroup=new H5::Group(file.createGroup("/Data"));
	H5::DataSet* dset=new H5::DataSet(dataGroup->createDataSet("Counts", H5::PredType::NATIVE_INT32, *space, *plist));

	hsize_t offset[3];
	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;

	for(offset[0]=0; offset[0]<_data.size(); offset[0] += count[0])
	{
	  space->selectHyperslab(H5S_SELECT_SET,count,offset,NULL,NULL);
	  dset->write(_data.at(offset[0]).data(),H5::PredType::NATIVE_INT32,*memspace,*space);
	}

	// Saving the scans parameters (detector angles and sample angles)

	H5::Group* scanGroup=new H5::Group(dataGroup->createGroup("Scan"));
	H5::Group* detectorGroup=new H5::Group(scanGroup->createGroup("Detector"));

	std::vector<std::string> names=_diffractometer->getDetector()->getGonio()->getPhysicalAxesNames();
	hsize_t nf[1]={_nFrames};
	H5::DataSpace scanSpace(1,nf);
	Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> vals(names.size(),_nFrames);

	for (unsigned int i=0;i<_detectorStates.size();++i)
	{
		const std::vector<double>& v=_detectorStates[i].getValues();
		for (unsigned int j=0;j<names.size();++j)
		{
			vals(j,i)=v[j]/SX::Units::deg;
		}
	}

	for (unsigned int j=0;j<names.size();++j)
	{
		H5::DataSet detectorScan(detectorGroup->createDataSet(names[j],H5::PredType::NATIVE_DOUBLE,scanSpace));
		detectorScan.write(&vals(j,0),H5::PredType::NATIVE_DOUBLE,scanSpace,scanSpace);
	}

	H5::Group* sampleGroup=new H5::Group(scanGroup->createGroup("Sample"));

	std::vector<std::string> samplenames=_diffractometer->getSample()->getGonio()->getPhysicalAxesNames();
	Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> valsSamples(samplenames.size(),_nFrames);

	for (unsigned int i=0;i<_sampleStates.size();++i)
	{
		const std::vector<double>& v=_sampleStates[i].getValues();
		for (unsigned int j=0;j<samplenames.size();++j)
		{
			valsSamples(j,i)=v[j]/SX::Units::deg;
		}
	}

	for (unsigned int j=0;j<samplenames.size();++j)
	{
		H5::DataSet sampleScan(sampleGroup->createDataSet(samplenames[j],H5::PredType::NATIVE_DOUBLE,scanSpace));
		sampleScan.write(&valsSamples(j,0),H5::PredType::NATIVE_DOUBLE,scanSpace,scanSpace);
	}

	const auto& map=_metadata->getMap();


	// Write all string metadata into the "Info" group
	H5::Group* infogroup=new H5::Group(file.createGroup("/Info"));
	H5::DataSpace metaSpace(H5S_SCALAR);
	H5::StrType str80(H5::PredType::C_S1, 80);
	std::string info;
	for (const auto& item : map)
	{
		std::string info;
		try
		{
			info=boost::any_cast<std::string>(item.second);
			H5::Attribute intAtt(infogroup->createAttribute(item.first,str80,metaSpace));
			intAtt.write(str80,info);
		}
		catch(...)
		{
		}
	}

	// Write all other metadata (int and double) into the "Experiment" Group
	H5::Group* metadatagroup=new H5::Group(file.createGroup("/Experiment"));
	//
	for (const auto& item : map)
	{
		int value;
		try
		{
			value=boost::any_cast<int>(item.second);
			H5::Attribute intAtt(metadatagroup->createAttribute(item.first,H5::PredType::NATIVE_INT32,metaSpace));
			intAtt.write(H5::PredType::NATIVE_INT,&value);
		}
		catch(...)
		{
			try
			{
				double dvalue;
				dvalue=boost::any_cast<double>(item.second);
				H5::Attribute intAtt(metadatagroup->createAttribute(item.first,H5::PredType::NATIVE_DOUBLE,metaSpace));
				intAtt.write(H5::PredType::NATIVE_DOUBLE,&dvalue);
			}catch(...)
			{

			}
		}

	}
	delete dataGroup;
	delete scanGroup;
	delete detectorGroup;
	delete sampleGroup;
	delete infogroup;
	delete metadatagroup;
	delete dset;
	delete memspace;
	delete space;
	delete plist;
	file.close();

	//
	blosc_destroy();
}



void IData::addMask(AABB<double,3>* mask)
{
	// Insert the mask
	_masks.insert(mask);
	// Update the peaks with this mask
	maskPeaks();
}

void IData::removeMask(AABB<double,3>* mask)
{
	auto p=_masks.find(mask);
	if (p!=_masks.end())
		_masks.erase(mask);
	// Update the peaks selection status.
	maskPeaks();
}

void IData::maskPeaks() const
{
	for (auto p : _peaks)
		maskPeak(p);
}

void IData::maskPeak(Peak3D* peak) const
{
	peak->setMasked(false);
	for (auto m : _masks)
	{
		// If the background of the peak intercept the mask, unselected the peak
		if (m->intercept(*(peak->getBackground())))
		{
			peak->setMasked(true);
			break;
		}
	}
}

bool IData::inMasked(const Eigen::Vector3d& point) const
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

void IData::releaseMemory()
{
	if (!_inMemory)
        return;

    for (auto& d : _data)
        d.resize(0,0);
    _data.clear();
	_data.shrink_to_fit();

    _inMemory=false;
}

Peak3D* IData::hasPeak(double h, double k, double l, const Matrix3d& BU)
{
	// Get q at rest
	Eigen::RowVector3d q=Eigen::RowVector3d(h,k,l)*BU;
	double normQ=q.norm();
	double sintheta=0.5*normQ*_diffractometer->getSource()->getWavelength();

	// Get q vectors for the two endpoints of the scans, this describes an arc
	auto q1=_diffractometer->getSample()->getGonio()->transform(q,_sampleStates.front().getValues());
	auto q2=_diffractometer->getSample()->getGonio()->transform(q,_sampleStates.back().getValues());

	// y coordinate of Q needs to be <0 somewhere in the scan for Bragg
	if (q1[1]>0 && q2[1]>0)
		return nullptr;

	double c=-sintheta*normQ;
	if (!(q1[1]<c && c<q2[1]) && !(q2[1]<c && c<q1[1]))
		return nullptr;

	// Spherical linear interpolation between q1 and q2 gives:
	// q=q1*sin(omega.(1-t))/sin(omega)+q2*sin(omega.t)/sin(omega) with cos(omega)=q1.q2
	double comega=q1.dot(q2)/q1.norm()/q2.norm();
	double omega=acos(comega);
	double somega=sin(omega);
	// q must be equal to =sintheta*normQ to be  in Bragg condition,
	// equivalent  to solving the equation of the type a*cos(omega.t)+b*sin(omega.t)=c
	double a=q1[1];
	double b=(q2[1]-comega*q1[1])/somega;
	double a2b2=sqrt(a*a+b*b);
	c/=a2b2;

	// No solution to the equation
	if (c<-1 || c>1)
		return nullptr;

	double asinc=asin(c);
	double alpha=atan2(a,b);

	// First solution
	double t=fmod(asinc-alpha,2*M_PI)/omega;
	// Second  solution
	if (t<0 || t >1)
		t=fmod(M_PI-asinc-alpha,2*M_PI)/omega;
	// No solutions
	if (t<0 || t>1)
		return nullptr;

	// The q-vector in Bragg condition
	q=q1*sin(omega*(1-t))/somega+q2*sin(omega*t)/somega;
	// kf vector
	q+=_diffractometer->getSource()->getki();
	// Get detector state at this point
	ComponentState dis=getDetectorInterpolatedState((_nFrames-1)*t);
	double px,py;
	// If hit detector, new peak
	if (_diffractometer->getDetector()->receiveKf(px,py,q,dis.getValues()))
	{
		Peak3D* newpeak=new Peak3D(this);
		newpeak->setMillerIndices(h,k,l);
		ComponentState cs=getSampleInterpolatedState((_nFrames-1)*t);
		newpeak->setSampleState(new ComponentState(cs));
		DetectorEvent de=_diffractometer->getDetector()->createDetectorEvent(px,py,dis.getValues());
		newpeak->setDetectorEvent(new DetectorEvent(de));
		newpeak->setSource(_diffractometer->getSource());
		Eigen::Vector3d center(px,py,(_nFrames-1)*t);
		Eigen::Vector3d eigenvalues(2,2,2);
		Eigen::Matrix3d eigenvectors;
		eigenvectors << 1,0,0,0,1,0,0,0,1;
		newpeak->setPeakShape(new SX::Geometry::Ellipsoid<double,3>(center,eigenvalues,eigenvectors));
		newpeak->setBackgroundShape(new SX::Geometry::Ellipsoid<double,3>(center,eigenvalues*2,eigenvectors));
		return newpeak;
	}

	return nullptr;

}

double IData::getBackgroundLevel() const
{
	if (!_inMemory)
		return 0;

	double mean=0;
	for (unsigned int i=0;i<_nFrames;++i)
	{
		mean+=_data[i].sum();
	}
	mean/=(_nFrames*_nrows*_ncols);
	//
	return mean;
}

} // end namespace Data

} // end namespace SX
