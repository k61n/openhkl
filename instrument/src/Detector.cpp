#include <iostream>
#include <stdexcept>
#include <cmath>
#include "Detector.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Units;

Detector::Detector()
: Component(), _nRows(0), _nCols(0),
  _width(0.0), _height(0.0),_mapping(nullptr)
{
}

Detector::Detector(const std::string& name):Component(name),_nRows(0), _nCols(0),
		  _width(0.0), _height(0.0)
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


Eigen::Vector3d Detector::getKf(double px, double py, double wave) const
{
	// Get the event position x,y,z, taking into account the Gonio current setting
	Eigen::Vector3d p=getEventPosition(px,py);
	p.normalize();
	return (p/wave);
}

Eigen::Vector3d Detector::getQ(double px, double py, const Eigen::Vector3d& si) const
{
	if (si.isZero())
		throw std::runtime_error("Detector:getQ incident wavevector is null");
	double wave=1.0/si.norm();
	Eigen::Vector3d q=getKf(px,py,wave);
	q-=si;
	return q;
}

void Detector::getGammaNu(double px, double py, double& gamma, double& nu)
{
	Eigen::Vector3d p=getEventPosition(px,py);
	gamma=atan2(p[1],p[0]);
	nu=asin(p[2]/p.norm());
}

double Detector::get2Theta(double px, double py, const Eigen::Vector3d& si) const
{
	Eigen::Vector3d p=getEventPosition(px,py);
	double proj=p.dot(si);
	return asin(proj/p.norm()/si.norm());
}

void Detector::setDataMapping(std::function<void(double,double,double&,double&)> rhs)
{
	_mapping=rhs;
}

void Detector::convertCoordinates(double px,double py,double& mx,double& my)
{
	if (_mapping!=nullptr)
		_mapping(px,py,mx,my);
	else
	{
		mx=px;my=py;
	}
	return;

}


} // End namespace Instrument

} // End namespace SX

