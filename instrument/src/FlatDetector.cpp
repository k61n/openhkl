#include <memory>

#include <boost/foreach.hpp>

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

void FlatDetector::buildFromXML(const property_tree::ptree& node)
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

	// Set the detector width from the XML node
	const property_tree::ptree& widthNode = node.get_child("width");
	units=um->get(widthNode.get<std::string>("<xmlattr>.units"));
	double width=widthNode.get_value<double>();
	width *= units;
    this->setWidth(width);

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
				throw Kernel::Error<FlatDetector>("Invalid axis type. Must be one of 'rotation' or 'translation'.");
        }
	}

    this->setGonio(gonio);
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

	px=(v[0]/_width+0.5)*(_nCols-1);
	py=(v[2]/_height+0.5)*(_nRows-1);

	if (px<0 || px > _nCols|| py<0 || py> _nRows)
		return false;

	return true;


}

} // Namespace Instrument

} // Namespace SX
