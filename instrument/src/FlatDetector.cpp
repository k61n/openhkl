#include <memory>

#include <Eigen/Dense>

#include "Error.h"
#include "FlatDetector.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "TransAxis.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

Detector* FlatDetector::create(const proptree::ptree& node)
{
	return new FlatDetector(node);
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

FlatDetector::FlatDetector(const proptree::ptree& node) : MonoDetector(node)
{
	Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	// Set the detector width from the property tree node
	const property_tree::ptree& widthNode = node.get_child("width");
	double units=um->get(widthNode.get<std::string>("<xmlattr>.units"));
	double width=widthNode.get_value<double>();
	width *= units;
    setWidth(width);

	// Set the detector height from the property tree node
	const property_tree::ptree& heightNode = node.get_child("height");
	units=um->get(heightNode.get<std::string>("<xmlattr>.units"));
	double height=heightNode.get_value<double>();
	height *= units;
    setHeight(height);

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
	result[0]=0.5*_width*((2*x+1.0)/_nCols-1);;
	result[1]=_distance;
	result[2]=0.5*_height*((2*y+1.0)/_nRows-1);
	return result;
}

bool FlatDetector::hasKf(const Eigen::Vector3d& kf,const Eigen::Vector3d& f, double& px, double& py) const
{
	double x=_distance-f[1];
	if (std::fabs(kf[1])<1e-10 || std::fabs(x)<1e-10)
	{
		px=0;
		py=0;
		return false;
	}
	double t=x/kf[1];
	auto v=f+kf*t;

	px=0.5*(_nCols*(2*v[0]/_width+1)-1);
	py=0.5*(_nRows*(2*v[2]/_height+1)-1);

	if (px<0 || px > _nCols|| py<0 || py> _nRows)
		return false;

	return true;
}

} // Namespace Instrument

} // Namespace SX
