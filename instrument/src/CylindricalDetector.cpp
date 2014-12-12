#include "CylindricalDetector.h"
#include "Gonio.h"

namespace SX
{

namespace Instrument
{

Detector* CylindricalDetector::create(const std::string& name)
{
	return new CylindricalDetector(name);
}

CylindricalDetector::CylindricalDetector() : Detector()
{
}

CylindricalDetector::CylindricalDetector(const CylindricalDetector& other) : Detector(other)
{
}

CylindricalDetector::CylindricalDetector(const std::string& name):Detector(name)
{

}

CylindricalDetector::~CylindricalDetector()
{
}

CylindricalDetector& CylindricalDetector::operator=(const CylindricalDetector& other)
{
	if (this != &other)
		Detector::operator=(other);
	return *this;
}

Detector* CylindricalDetector::clone() const
{
	return new CylindricalDetector(*this);
}

void CylindricalDetector::setAngularRange(double w, double h)
{
	_widthAngle=w;
	_heightAngle=h;
	// R.dTheta
	_width=w*_distance;
	// Full height
	_height=2.0*_distance*tan(h);
}

void CylindricalDetector::setWidthAngle(double wangle)
{
	_widthAngle=wangle;
	// R.dTheta
	_width=wangle*_distance;
}

void CylindricalDetector::setHeightAngle(double hangle)
{
	_heightAngle=hangle;
	_height=2.0*_distance*tan(hangle);
}

Eigen::Vector3d CylindricalDetector::getPos(double px, double py) const
{
	if (_nCols==0 || _nRows==0)
		throw std::runtime_error("Detector: number of rows or cols must >0");

	if (_height==0 || _width==0)
		throw std::runtime_error("Detector: width or height must be >0");

	if (_distance==0)
		throw std::runtime_error("Detector: distance must be >0");

	// The coordinates are defined relatively to the detector origin
	double x=px-_minCol;
	double y=py-_minRow;

	double gamma=(x/(_nCols-1.0)-0.5)*_widthAngle;
	Eigen::Vector3d result;
	result[0]=_distance*sin(gamma);
	// Angle
	result[1]=_distance*cos(gamma);
	// take the center of the bin
	result[2]=(y/(_nRows-1.0)-0.5)*_height;
	return result;
}

}
}
