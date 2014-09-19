#include <cmath>
#include <stdexcept>
#include "IData.h"
#include "Peak3D.h"
#include "Diffractometer.h"
#include "ComponentState.h"
#include "DetectorEvent.h"
#include "Detector.h"
#include "Sample.h"

namespace SX
{
namespace Crystal
{

Peak3D::Peak3D(SX::Data::IData* data):
		_data(data),
		_hkl(Eigen::Vector3d::Zero()),
		_peak(nullptr),
		_bkg(nullptr),
		_scale(1.0),
		_sampleState(nullptr),
		_event(nullptr),
		_wave(0.0)
{
}

Peak3D::~Peak3D()
{
	//if (_peak)
	//	delete _peak;
	//if (_bkg)
	//	delete _bkg;
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

		for (int x=start_x;x<=end_x;++x)
		{
			for (int y=start_y;y<=end_y;++y)
			{
				int intensity=_data->dataAt(x,y,z);
				_projection[z-data_start]+=intensity;
				point1 << x,y,z,1;
				point2 << x+1,y,z,1;
				point3 << x,y+1,z,1;
				point4 << x+1,y+1,z,1;
				bool inpeak=(_peak->isInside(point1) || _peak->isInside(point2) || _peak->isInside(point3) || _peak->isInside(point4));
				if (inpeak)
					_projectionPeak[z-data_start]+=intensity;
			}
		}
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
	// Add one point left and right of the peak in omega
	int max=_data->_mm->getMetaData()->getKey<int>("npdone");
	if (datastart>1)
		datastart--;
	if (dataend<max-1)
		dataend++;

	// Safety check
	if (datastart==dataend)
		return;

	double bkg_left=bkg[datastart];
	double bkg_right=bkg[dataend];
	double diff;
	for (int i=datastart;i<=dataend;++i)
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
	if (std::fabs(_hkl[0]-std::round(_hkl[0]))<0.12 && std::fabs(_hkl[1]-std::round(_hkl[1]))<0.12 && std::fabs(_hkl[2]-std::round(_hkl[2]))<0.12)
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

Eigen::RowVector3d Peak3D::getQ() const
{
	return _event->getParent()->getQ(*_event,_wave,_sampleState->getParent()->getPosition(*_sampleState));
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


}
}

