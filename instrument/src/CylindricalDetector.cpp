#include <memory>

#include <boost/foreach.hpp>

#include <Eigen/Dense>

#include "CylindricalDetector.h"
#include "Error.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "TransAxis.h"
#include "Units.h"

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

void CylindricalDetector::buildFromXML(const property_tree::ptree& node)
{

	Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

	// Set the detector name from the XML node
	std::string detectorName=node.get<std::string>("name");
	this->setName(detectorName);

	// Set the detector to sample distance from the XML node
	const property_tree::ptree& distanceNode = node.get_child("sample_distance");
	double units=um->get(distanceNode.get<std::string>("<xmlattr>.units"));
	double distance=distanceNode.get_value<double>();
	distance *= units;
    this->setDistance(distance);

	// Set the detector angular width from the XML node
	const property_tree::ptree& angularWidthNode = node.get_child("angular_width");
	units=um->get(angularWidthNode.get<std::string>("<xmlattr>.units"));
	double angularWidth=angularWidthNode.get_value<double>();
	angularWidth *= units;
    this->setAngularWidth(angularWidth);

	// Set the detector height from the XML node
	const property_tree::ptree& heightNode = node.get_child("height");
	units=um->get(heightNode.get<std::string>("<xmlattr>.units"));
	double height=heightNode.get_value<double>();
	height *= units;
    this->setHeight(height);

	// Set the detector number of pixels from the XML node
	const property_tree::ptree& nColsNode = node.get_child("ncols");
	unsigned int nCols=nColsNode.get_value<unsigned int>();
	const property_tree::ptree& nRowsNode = node.get_child("nrows");
	unsigned int nRows=nRowsNode.get_value<unsigned int>();
    this->setNPixels(nCols,nRows);

    // Set the detector goniometer from the XML node

    const property_tree::ptree& goniometerNode=node.get_child("goniometer");
    std::string goniometerName=goniometerNode.get<std::string>("name");
    std::shared_ptr<Gonio> gonio(new Gonio(goniometerName));

    // Set the axis of the detector goniometer from the XML node
	BOOST_FOREACH(const property_tree::ptree::value_type& v, goniometerNode)
	{
	    if (v.first.compare("axis")==0)
	    {
	    	std::string axisType=v.second.get<std::string>("<xmlattr>.type");
	    	std::string axisName=v.second.get<std::string>("name");

	    	const property_tree::ptree& axisDirectionNode=v.second.get_child("direction");
	    	double nx=axisDirectionNode.get<double>("x");
	    	double ny=axisDirectionNode.get<double>("y");
	    	double nz=axisDirectionNode.get<double>("z");

	    	Eigen::Vector3d axisDir(nx,ny,nz);
	    	axisDir.normalize();

	    	bool physical=v.second.get<bool>("physical");
	    	double offset=v.second.get<double>("offset");

	    	// Case of a rotation axis
	    	if (axisType.compare("rotation")==0)
	    	{
	    		bool clockwise=v.second.get<bool>("clockwise");
	    		RotAxis::Direction sense=clockwise ? RotAxis::Direction::CW : RotAxis::Direction::CCW;
		        gonio->addRotation(axisName,axisDir,sense);
	    		gonio->getAxis(axisName)->setPhysical(physical);
	    		gonio->getAxis(axisName)->setOffset(offset);
	    	}
	    	// Case of a translation axis
	    	else if (axisType.compare("translation")==0)
	    	{
		        gonio->addTranslation(axisName,axisDir);
	    		gonio->getAxis(axisName)->setPhysical(physical);
	    		gonio->getAxis(axisName)->setOffset(offset);
	    	}
	    	else
				throw Kernel::Error<CylindricalDetector>("Invalid axis type. Must be one of 'rotation' or 'translation'.");
        }
	}

    this->setGonio(gonio);

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

bool CylindricalDetector::hasKf(const Eigen::Vector3d& kf,const Eigen::Vector3d& f, double& px, double& py) const
{

	// Need to solve equation of the typr (from_xy + f_xy*t)^2=R^2
	double b=2*(f[0]*kf[0]+f[1]*kf[1]);
	double a=(kf[0]*kf[0]+kf[1]*kf[1]);
	double c= f[0]*f[0]+f[1]*f[1]-_distance*_distance;

	double Delta=b*b-4*a*c;
	if (Delta<0)
		return false;

	Delta=sqrt(Delta);

	double t=0.5*(-b+Delta)/a;
	if (t<=0)
		return false;

	auto v=f+kf*t;

	double phi=atan2(v[0],v[1])+0.5*_angularWidth;
	if (phi<0 || phi>=_angularWidth)
		return false;

	double d=v[2]/_height+0.5;

	if (d<0 || d>1.0)
		return false;

	px=phi/_angularWidth*(_nCols-1);
	py=d*(_nRows-1);
	return true;
}

} // Namespace Instrument

} // Namespace SX
