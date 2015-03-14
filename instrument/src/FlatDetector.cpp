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

FlatDetector::FlatDetector() : MonoDetector()
{
}

FlatDetector::FlatDetector(const FlatDetector& other) : MonoDetector(other)
{
}

FlatDetector::FlatDetector(const std::string& name) : MonoDetector(name)
{
}

FlatDetector::~FlatDetector()
{
}

FlatDetector& FlatDetector::operator=(const FlatDetector& other)
{
	if (this != &other)
		MonoDetector::operator=(other);

	return *this;
}

Detector* FlatDetector::clone() const
{
	return new FlatDetector(*this);
}

void FlatDetector::setWidth(double width)
{
	_width=width;
	_angularWidth=2.0*atan(0.5*_width/_distance);
}

void FlatDetector::setHeight(double height)
{
	_height=height;
	_angularHeight = 2.0*atan(0.5*_height/_distance);
}

void FlatDetector::setAngularWidth(double angle)
{
	_angularWidth=angle;
	_width=2.0*_distance*tan(angle);
}

void FlatDetector::setAngularHeight(double angle)
{
	_angularHeight = angle;
	_height=2.0*_distance*tan(angle);
}

Eigen::Vector3d FlatDetector::getPos(double px, double py) const
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

	Eigen::Vector3d result;
	// take the center of the bin
	result[0]=(x/(_nCols-1.0)-0.5)*_width;
	result[1]=_distance;
	result[2]=(y/(_nRows-1.0)-0.5)*_height;
	return result;
}

bool FlatDetector::hasKf(const Eigen::Vector3d& kf, double& px, double& py) const
{
	if (kf[1]<1e-10)
	{
		px=0;
		py=0;
		return false;
	}
	auto kf2=kf;
	kf2.normalize();
	kf2*=_distance/kf[1];
	px=(kf2[0]+0.5*_width)/_width*(_nCols-1);
	py=(kf2[2]+0.5*_height)/_height*(_nRows-1);
	if (px<0 || px > _nCols|| py<0 || py> _nRows)
		return false;

	return true;


}

} // Namespace Instrument

} // Namespace SX
