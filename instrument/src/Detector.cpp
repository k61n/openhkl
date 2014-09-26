#include <iostream>
#include <stdexcept>
#include <cmath>
#include "Detector.h"
#include "Units.h"
#include "Gonio.h"
namespace SX
{

namespace Instrument
{

using namespace SX::Units;

Detector::Detector()
: Component(), _nRows(0), _nCols(0),
  _width(0.0), _height(0.0),_distance(0),_mapping(nullptr)
{
}

Detector::Detector(const std::string& name):Component(name),_nRows(0), _nCols(0),
		  _width(0.0), _height(0.0),_distance(0),_mapping(nullptr)
{

}

Detector::~Detector()
{

}

void Detector::setDimensions(double width, double height)
{
	if (width<=0 || height<=0)
		throw std::range_error("Detector "+Component::_name+" dimensions must be positive");
	_width=width;
	_height=height;
}

void Detector::setWidth(double width)
{
	if (width<=0)
		throw std::range_error("Detector "+Component::_name+" dimensions must be positive");
	_width=width;
}

void Detector::setHeight(double height)
{
	if (height<=0)
		throw std::range_error("Detector "+Component::_name+" dimensions must be positive");
	_height=height;
}

void Detector::setDistance(double d)
{
	_distance=d;
	_position=Eigen::Vector3d(0,d,0);
}
void Detector::setRestPosition(const Eigen::Vector3d& pos)
{
	_position=pos;
	_distance=pos.norm();
}

double Detector::getHeigth() const
{
	return _height;
}

double Detector::getWidth() const
{
	return _width;
}

void Detector::setNPixels(unsigned int cols, unsigned int rows)
{
	if (cols==0 || rows==0)
		throw std::range_error("Detector "+Component::_name+" number of pixels (row,col) must be >0");
	_nCols=cols;
	_nRows=rows;
}
void Detector::setNCols(unsigned int cols)
{
	if (cols==0)
		throw std::range_error("Detector "+Component::_name+" number of pixels (row,col) must be >0");
	_nCols=cols;
}

void Detector::setNRows(unsigned int rows)
{
	if (rows==0)
		throw std::range_error("Detector "+Component::_name+" number of pixels (row,col) must be >0");
	_nRows=rows;
}

int Detector::getNCols() const
{
	return _nCols;
}

int Detector::getNRows() const
{
	return _nRows;
}

double Detector::getPixelHeigth() const
{
	return _height/_nRows;
}

double Detector::getPixelWidth() const
{
	return _width/_nCols;
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
	else if (_gonio->numberOfAxes()!=event._values.size())
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
	q-=Eigen::Vector3d(0.0,1/wave,0.0);
	return q;
}

void Detector::getGammaNu(double px, double py, double& gamma, double& nu,const std::vector<double>& values,const Eigen::Vector3d& from) const
{
	Eigen::Vector3d p=getEventPosition(px,py,values)-from;
	gamma=atan2(p[0],p[1]);
	nu=asin(p[2]/p.norm());
}

void Detector::getGammaNu(const DetectorEvent& event, double& gamma, double& nu,const Eigen::Vector3d& from) const
{
	Eigen::Vector3d p=getEventPosition(event)-from;
	gamma=atan2(p[0],p[1]);
	nu=asin(p[2]/p.norm());
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

void Detector::setDataMapping(std::function<void(double,double,double&,double&)> rhs)
{
	_mapping=rhs;
}

void Detector::convertCoordinates(double px,double py,double& mx,double& my) const
{
	if (_mapping!=nullptr)
		_mapping(px,py,mx,my);
	else
	{
		mx=px;my=py;
	}
	return;

}

DetectorEvent Detector::createDetectorEvent(double x, double y, const std::vector<double>& values)
{
	if (!_gonio)
	{
		if (values.size())
			throw std::runtime_error("Trying to create a DetectorEvent with Goniometer values whilst no gonio is set");
	}
	else if (values.size()!=_gonio->numberOfAxes())
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

