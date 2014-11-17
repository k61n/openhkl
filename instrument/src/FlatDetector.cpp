#include "FlatDetector.h"
#include "Gonio.h"

namespace SX
{

namespace Instrument
{

Detector* FlatDetector::create(const std::string& name)
{
	return new FlatDetector(name);
}

FlatDetector::FlatDetector() : Detector()
{
}

FlatDetector::FlatDetector(const FlatDetector& other) : Detector(other)
{
}

FlatDetector::FlatDetector(const std::string& name):Detector(name)
{
}

FlatDetector::~FlatDetector()
{
}

FlatDetector& FlatDetector::operator=(const FlatDetector& other)
{
	if (this != &other)
		Detector::operator=(other);

	return *this;
}

Detector* FlatDetector::clone() const
{
	return new FlatDetector(*this);
}

void FlatDetector::setAngularRange(double w, double h)
{
	// Full width
	_width=2.0*_distance*tan(w);
	// Full height
	_height=2.0*_distance*tan(h);
}

void FlatDetector::setWidthAngle(double wangle)
{
	_width=2.0*_distance*tan(wangle);
}

void FlatDetector::setHeightAngle(double hangle)
{
	_height=2.0*_distance*tan(hangle);
}

double FlatDetector::getWidthAngle()const
{
	return 2.0*atan(0.5*_width/_distance);
}

double FlatDetector::getHeightAngle() const
{
	return 2.0*atan(0.5*_height/_distance);
}

Eigen::Vector3d FlatDetector::getPos(double px, double py) const
{
	if (_nCols==0 || _nRows==0)
		throw std::runtime_error("Detector: number of rows or cols must >0");

	if (_height==0 || _width==0)
		throw std::runtime_error("Detector: width or height must be >0");

	if (_distance==0)
		throw std::runtime_error("Detector: distance must be >0");

	// Convert coordinates due to detector mapping,
	// mx,my nows in the internal convention
	double mx,my;
	convertCoordinates(px,py,mx,my);
	Eigen::Vector3d result;
	// take the center of the bin
	result[0]=(mx/(_nCols-1.0)-0.5)*_width;
	result[1]=_distance;
	result[2]=(my/(_nRows-1.0)-0.5)*_height;
	return result;
}

}
}
