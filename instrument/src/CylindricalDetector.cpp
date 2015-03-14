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

CylindricalDetector::CylindricalDetector() : MonoDetector()
{
}

CylindricalDetector::CylindricalDetector(const CylindricalDetector& other) : MonoDetector(other)
{
}

CylindricalDetector::CylindricalDetector(const std::string& name) : MonoDetector(name)
{

}

CylindricalDetector::~CylindricalDetector()
{
}

CylindricalDetector& CylindricalDetector::operator=(const CylindricalDetector& other)
{
	if (this != &other)
		MonoDetector::operator=(other);
	return *this;
}

Detector* CylindricalDetector::clone() const
{
	return new CylindricalDetector(*this);
}

void CylindricalDetector::setHeight(double height)
{
	_height=height;
	_angularHeight=2.0*atan(0.5*_height/_distance);
}

void CylindricalDetector::setWidth(double width)
{
	_width=width;
	_angularWidth=_width/_distance;
}

void CylindricalDetector::setAngularHeight(double angle)
{
	_angularHeight=angle;
	_height=2.0*_distance*tan(angle);
}

void CylindricalDetector::setAngularWidth(double angle)
{
	_angularWidth=angle;
	// R.dTheta
	_width=_angularWidth*_distance;
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

	double gamma=(x/(_nCols-1.0)-0.5)*_angularWidth;
	Eigen::Vector3d result;
	result[0]=_distance*sin(gamma);
	// Angle
	result[1]=_distance*cos(gamma);
	// take the center of the bin
	result[2]=(y/(_nRows-1.0)-0.5)*_height;
	return result;
}

bool CylindricalDetector::hasKf(const Eigen::Vector3d& kf, double& px, double& py) const
{
	double phi=atan2(kf[0],kf[1])+0.5*_angularWidth;
	if (phi<0 || phi>=_angularWidth)
		return false;

	double inplane=sqrt(kf[0]*kf[0]+kf[1]*kf[1]);
	if (inplane<1e-10)
		return false;
	double d=_distance*kf[2]/inplane+0.5*_height;
	if (d<0 || d>_height)
		return false;

	px=phi/_angularWidth*(_nCols-1);
	py=d/_height*(_nRows-1);
	return true;
}

} // Namespace Instrument

} // Namespace SX
