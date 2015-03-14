#include <stdexcept>
#include <cmath>

#include "Detector.h"
#include "Gonio.h"

namespace SX
{

namespace Instrument
{

Detector::Detector() : Component("detector")
{
}

Detector::Detector(const Detector& other)
: Component(other)
{
}

Detector::Detector(const std::string& name)
: Component(name)
{
}

Detector::~Detector()
{
}

Detector& Detector::operator=(const Detector& other)
{
	if (this!=&other)
		Component::operator=(other);
	return *this;
}

Eigen::Vector3d Detector::getEventPosition(double px, double py, const std::vector<double>& values) const
{
	Eigen::Vector3d v=getPos(px,py);
	if (_gonio)
		_gonio->transformInPlace(v,values);
	return v;
}

Eigen::Vector3d Detector::getEventPosition(const DetectorEvent& event) const
{
	if (event._detector!=this)
		throw std::runtime_error("Trying to assign DetectorEvent to a different detector");
	Eigen::Vector3d v=getPos(event._x,event._y);
	// No gonio and no values set
	if (!_gonio)
	{
		if (event._values.size())
			throw std::runtime_error("Trying to assign a DetectorEvent with values to a Component with no Goniometer");
		else
			return v;
	}
	else if (_gonio->getNPhysicalAxes()!=event._values.size())
	{
		throw std::runtime_error("Trying to assign a DetectorEvent with wrong number of values");
	}
	_gonio->transformInPlace(v,event._values);
	return v;
}

Eigen::Vector3d Detector::getKf(double px, double py, double wave,const std::vector<double>& values,const Eigen::Vector3d& from) const
{
	// Get the event position x,y,z, taking into account the Gonio current setting
	Eigen::Vector3d p=getEventPosition(px,py,values);
	p-=from;
	p.normalize();
	return (p/wave);
}

Eigen::Vector3d Detector::getKf(const DetectorEvent& event, double wave,const Eigen::Vector3d& from) const
{
	// Get the event position x,y,z, taking into account the Gonio current setting
	Eigen::Vector3d p=getEventPosition(event);
	p-=from;
	p.normalize();
	return (p/wave);
}

bool Detector::receiveKf(double& px, double& py, const Eigen::Vector3d& kf,const std::vector<double>& goniovalues)
{

	if (_gonio)
	{
		auto kft=_gonio->transformInverse(kf,goniovalues);
		return hasKf(kft,px,py);
	}
	else
		return hasKf(kf,px,py);
}

Eigen::Vector3d Detector::getQ(double px, double py,double wave,const std::vector<double>& values,const Eigen::Vector3d& from) const
{
	if (wave<=0)
		throw std::runtime_error("Detector:getQ incident wavelength error, must be >0");
	Eigen::Vector3d q=getKf(px,py,wave,values,from);
	q-=Eigen::Vector3d(0.0,1/wave,0.0);
	return q;
}

Eigen::Vector3d Detector::getQ(const DetectorEvent& event,double wave,const Eigen::Vector3d& from) const
{
	if (wave<=0)
		throw std::runtime_error("Detector:getQ incident wavelength error, must be >0");
	Eigen::Vector3d q=getKf(event,wave,from);
	q[1]-=1.0/wave;
	return q;
}

void Detector::getGammaNu(double px, double py, double& gamma, double& nu,const std::vector<double>& values,const Eigen::Vector3d& from) const
{
	Eigen::Vector3d p=getEventPosition(px,py,values)-from;
	gamma=std::atan2(p[0],p[1]);
	nu=std::asin(p[2]/p.norm());
}

void Detector::getGammaNu(const DetectorEvent& event, double& gamma, double& nu,const Eigen::Vector3d& from) const
{
	Eigen::Vector3d p=getEventPosition(event)-from;
	gamma=std::atan2(p[0],p[1]);
	nu=std::asin(p[2]/p.norm());
}


double Detector::get2Theta(double px, double py,const std::vector<double>& values,const Eigen::Vector3d& si) const
{
	Eigen::Vector3d p=getEventPosition(px,py,values);
	double proj=p.dot(si);
	return acos(proj/p.norm()/si.norm());
}

double Detector::get2Theta(const DetectorEvent& event, const Eigen::Vector3d& si) const
{
	Eigen::Vector3d p=getEventPosition(event);
	double proj=p.dot(si);
	return acos(proj/p.norm()/si.norm());
}

DetectorEvent Detector::createDetectorEvent(double x, double y, const std::vector<double>& values)
{
	if (!_gonio)
	{
		if (values.size())
			throw std::runtime_error("Trying to create a DetectorEvent with Goniometer values whilst no gonio is set");
	}
	else if (values.size()!=_gonio->getNPhysicalAxes())
		throw std::runtime_error("Trying to create a DetectorEvent with invalid number of Goniometer Axes");

	DetectorEvent result;
	result._detector=this;
	result._x=x;
	result._y=y;
	result._values=values;
	return result;
}

} // End namespace Instrument

} // End namespace SX

