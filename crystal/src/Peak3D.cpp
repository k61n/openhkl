#include <cmath>
#include <stdexcept>
#include "IData.h"
#include "Peak3D.h"
#include "Diffractometer.h"
#include "ComponentState.h"
#include "DetectorEvent.h"
#include "Detector.h"
#include "Sample.h"
#include "IShape.h"
#include "Gonio.h"

namespace SX
{
namespace Crystal
{

Peak3D::Peak3D(SX::Data::IData* data):
		_data(data),
		_hkl(Eigen::Vector3d::Zero()),
		_peak(nullptr),
		_bkg(nullptr),
		_sampleState(nullptr),
		_event(nullptr),
		_counts(0.0),
		_countsSigma(0.0),
		_wave(0.0),
		_scale(1.0),
		_selected(true),
		_masked(false)
{
}

Peak3D::Peak3D(const Peak3D& other):
		_data(other._data),
		_hkl(other._hkl),
		_peak(other._peak == nullptr ? nullptr : other._peak->clone()),
		_bkg(other._bkg == nullptr ? nullptr : other._bkg->clone()),
		_sampleState(other._sampleState),
		_event(other._event),
		_counts(other._counts),
		_countsSigma(other._countsSigma),
		_wave(other._wave),
		_scale(other._scale),
		_selected(other._selected)
{
}

Peak3D& Peak3D::operator=(const Peak3D& other)
{

	if (this != &other)
	{

		_data = other._data;
		_hkl = other._hkl;

		if (other._peak == nullptr)
			_peak = nullptr;
		else
			_peak = other._peak->clone();

		if (other._bkg == nullptr)
			_bkg = nullptr;
		else
			_bkg = other._bkg->clone();

		_sampleState = other._sampleState;
		_event = other._event;
		_counts = other._counts;
		_countsSigma = other._countsSigma;
		_wave = other._wave;
		_scale = other._scale;
		_selected = other._selected;

	}

	return *this;

}

Peak3D::~Peak3D()
{
    if (_peak)
        delete _peak;
    if (_bkg)
        delete _bkg;
}

void Peak3D::linkData(SX::Data::IData* data)
{
	_data=data;
}

void Peak3D::unlinkData()
{
	_data=nullptr;
}

void Peak3D::setPeakShape(SX::Geometry::IShape<double,3>* p)
{
	_peak=p;
}

void Peak3D::setBackgroundShape(SX::Geometry::IShape<double,3>* b)
{
	_bkg=b;
}


void Peak3D::setMillerIndices(double h, double k, double l)
{
	_hkl << h,k,l;
}
const Eigen::RowVector3d& Peak3D::getMillerIndices() const
{
	return _hkl;
}

void Peak3D::integrate()
{
	if (!_data)
		return;
	// Get the lower and upper limit of the bkg Bounding box
	const Eigen::Vector3d& lower=_bkg->getLower();
	const Eigen::Vector3d& upper=_bkg->getUpper();

	//
	int data_start=static_cast<int>(std::floor(lower[2]));
	int data_end=static_cast<int>(std::ceil(upper[2]));

	int start_x=static_cast<int>(std::floor(lower[0]));
	int end_x=static_cast<int>(std::ceil(upper[0]));

	int start_y=static_cast<int>(std::floor(lower[1]));
	int end_y=static_cast<int>(std::ceil(upper[1]));

	Eigen::Vector4d point1, point2, point3, point4;

	// Allocate all vectors
	_projection=Eigen::VectorXd::Zero(data_end-data_start+1);
	_projectionPeak=Eigen::VectorXd::Zero(data_end-data_start+1);
	_projectionBkg=Eigen::VectorXd::Zero(data_end-data_start+1);

	for (int z=data_start;z<=data_end;++z)
	{
		double pointsinpeak=0;
		double pointsinbkg=0;
		double intensityP=0;
		double intensityBkg=0;
		for (int x=start_x;x<=end_x;++x)
		{
			for (int y=start_y;y<=end_y;++y)
			{
				int intensity=_data->dataAt(y,x,z);
				_projection[z-data_start]+=intensity;
				point1 << x,y,z,1;
				point2 << x+1,y,z,1;
				point3 << x,y+1,z,1;
				point4 << x+1,y+1,z,1;
				bool inpeak=(_peak->isInside(point1) || _peak->isInside(point2) || _peak->isInside(point3) || _peak->isInside(point4));
				if (inpeak)
				{
					intensityP+=intensity;
					pointsinpeak++;
				}
				bool inbackground=(_bkg->isInside(point1) || _bkg->isInside(point2) || _bkg->isInside(point3) || _bkg->isInside(point4));
				if (inbackground && !inpeak)
				{
					intensityBkg+=intensity;
					pointsinbkg++;
				}
			}
		}
		if (pointsinpeak>0)
			_projectionPeak[z-data_start]=intensityP-intensityBkg*pointsinpeak/pointsinbkg;

	}

	// Quick fix determine the limits of the peak range
	int datastart=0;
	int dataend=0;
	bool startfound=false;
	for (int i=0;i<_projectionPeak.size();++i)
	{
		if (!startfound && std::fabs(_projectionPeak[i])>1e-6)
		{
			datastart=i;
			startfound=true;
		}
		if (startfound)
		{
			if (std::fabs(_projectionPeak[i])<1e-6)
			{
				dataend=i;
				break;
			}
		}

	}
	//

	Eigen::VectorXd bkg=_projection-_projectionPeak;
	if (datastart>1)
		datastart--;

	// Safety check
	if (datastart==dataend)
		return;

	double bkg_left=bkg[datastart];
	double bkg_right=bkg[dataend];
	double diff;
    for (int i=datastart;i<dataend;++i)
    {
		diff=bkg[i]-(bkg_left+static_cast<double>((i-datastart))/static_cast<double>((dataend-datastart))*(bkg_right-bkg_left));
		if (diff>0)
			_projectionPeak[i]+=diff;
	}
	_projectionBkg=_projection-_projectionPeak;

	_counts = _projectionPeak.sum();
	_countsSigma = std::sqrt(_counts);


	return;
}

Eigen::VectorXd Peak3D::getProjection() const
{
	return _scale*_projection;
}

Eigen::VectorXd Peak3D::getPeakProjection() const
{
	return _scale*_projectionPeak;
}

Eigen::VectorXd Peak3D::getBkgProjection() const
{
	return _scale*_projectionBkg;
}

Eigen::VectorXd Peak3D::getProjectionSigma() const
{
	return _scale*(_projection.array().sqrt());
}

Eigen::VectorXd Peak3D::getPeakProjectionSigma() const
{
	return _scale*(_projectionPeak.array().sqrt());
}

Eigen::VectorXd Peak3D::getBkgProjectionSigma() const
{
	return _scale*(_projectionBkg.array().sqrt());
}

bool Peak3D::setBasis(std::shared_ptr<SX::Geometry::Basis> basis)
{
	_basis=basis;
	_hkl=_basis->fromReciprocalStandard(this->getQ());
	if (std::fabs(_hkl[0]-std::round(_hkl[0]))<0.2 && std::fabs(_hkl[1]-std::round(_hkl[1]))<0.2 && std::fabs(_hkl[2]-std::round(_hkl[2]))<0.2)
	{
		_hkl[0]=std::round(_hkl[0]);
		_hkl[1]=std::round(_hkl[1]);
		_hkl[2]=std::round(_hkl[2]);
		return true;
	}
	return false;
}

bool Peak3D::hasIntegerHKL(std::shared_ptr<SX::Geometry::Basis> basis)
{
	_hkl=basis->fromReciprocalStandard(this->getQ());
	if (std::fabs(_hkl[0]-std::round(_hkl[0]))<0.2 && std::fabs(_hkl[1]-std::round(_hkl[1]))<0.2 && std::fabs(_hkl[2]-std::round(_hkl[2]))<0.2)
	{
		_hkl[0]=std::round(_hkl[0]);
		_hkl[1]=std::round(_hkl[1]);
		_hkl[2]=std::round(_hkl[2]);
		return true;
	}
	return false;
}


double Peak3D::getRawIntensity() const
{
	return _counts;
}

double Peak3D::getScaledIntensity() const
{
	return _scale*_counts;
}

double Peak3D::getRawSigma() const
{
	return _countsSigma;
}

double Peak3D::getScaledSigma() const
{
	return _scale*_countsSigma;
}

double Peak3D::getLorentzFactor() const
{
	double gamma,nu;
	this->getGammaNu(gamma,nu);
	return 1.0/(sin(std::fabs(gamma))*cos(nu));
}

double Peak3D::getScale() const
{
	return _scale;
}

void Peak3D::rescale(double scale)
{
	_scale *= scale;
}

void Peak3D::setScale(double scale)
{
	_scale = scale;

}

SX::Instrument::DetectorEvent* Peak3D::getDetectorEvent()
{
	return _event;
}

SX::Instrument::ComponentState* Peak3D::getSampleState()
{
	return _sampleState;
}

Eigen::RowVector3d Peak3D::getQ() const
{
	// If sample state is not set, assume sample is at the origin
	if (!_sampleState)
	{
		return _event->getParent()->getQ(*_event,_wave);
	}
	else // otherwise scattering point is deducted from the sample
	{
		Eigen::Vector3d q=_event->getParent()->getQ(*_event,_wave,_sampleState->getParent()->getPosition(*_sampleState));
		_sampleState->getParent()->getGonio()->transformInverseInPlace(q,_sampleState->getValues());
		return q;
	}
}

void Peak3D::setWavelength(double wave)
{
	_wave=wave;
}

void Peak3D::setSampleState(SX::Instrument::ComponentState* sstate)
{
	_sampleState=sstate;
}

void Peak3D::setDetectorEvent(SX::Instrument::DetectorEvent* event)
{
	_event=event;
}

void Peak3D::getGammaNu(double& gamma,double& nu) const
{
	_event->getParent()->getGammaNu(*_event,gamma,nu,_sampleState->getParent()->getPosition(*_sampleState));
}

bool operator<(const Peak3D& p1, const Peak3D& p2)
{
	if (p1._hkl[0]<p2._hkl[0])
		return true;
	else if (p1._hkl[0]>p2._hkl[0])
		return false;
	if (p1._hkl[1]<p2._hkl[1])
		return true;
	else if (p1._hkl[1]>p2._hkl[1])
		return false;
	if (p1._hkl[2]<p2._hkl[2])
		return true;
	else if (p1._hkl[2]>p2._hkl[2])
		return false;
	return false;
}

bool Peak3D::isSelected() const
{
	return (!_masked && _selected);
}

void Peak3D::setSelected(bool s)
{
	_selected=s;
}

void Peak3D::setSample(SX::Instrument::Sample* sample)
{
	_sampleState->setParent(sample);
}
void Peak3D::setDetector(SX::Instrument::Detector* detector)
{
	_event->setParent(detector);
}

void Peak3D::setMasked(bool masked)
{
	_masked=masked;
}

bool Peak3D::isMasked() const
{
	return _masked;
}

}
}

