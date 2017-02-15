#include <cmath>
#include <stdexcept>

#include "Detector.h"
#include "DetectorEvent.h"
#include "DetectorFactory.h"
#include "Gonio.h"

namespace SX {
namespace Instrument {


Detector* Detector::create(const proptree::ptree& node)
{
    // Create an instance of the detector factory
    DetectorFactory* detectorFactory=DetectorFactory::Instance();

    // Get the detector type
    std::string detectorType=node.get<std::string>("<xmlattr>.type");

    // Fetch the detector from the factory
    Detector* detector = detectorFactory->create(detectorType,node);

    return detector;
}

Detector::Detector()
: Component("detector"),
  _dataorder(DataOrder::BottomLeftColMajor)
{
}

Detector::Detector(const Detector& other)
: Component(other),
  _dataorder(DataOrder::BottomLeftColMajor)
{
}

Detector::Detector(const std::string& name)
: Component(name),
  _dataorder(DataOrder::BottomLeftColMajor)
{
}

Detector::Detector(const proptree::ptree& node)
: Component(node)
{
    boost::optional<const proptree::ptree&> dataOrdernode=node.get_child_optional("data_ordering");
    // If data order is not defined assumed default
    if (!dataOrdernode)
    {
        _dataorder=DataOrder::BottomRightColMajor;
        return;
    }

    std::string dataOrder=dataOrdernode.get().get_value<std::string>();

    if (dataOrder.compare("TopLeftColMajor")==0)
    {
        _dataorder=DataOrder::TopLeftColMajor;
    }
    else if (dataOrder.compare("TopLeftRowMajor")==0)
    {
        _dataorder=DataOrder::TopLeftRowMajor;
    }
    else if (dataOrder.compare("TopRightColMajor")==0)
    {
        _dataorder=DataOrder::TopRightColMajor;
    }
    else if (dataOrder.compare("TopRightRowMajor")==0)
    {
        _dataorder=DataOrder::TopRightRowMajor;
    }
    else if (dataOrder.compare("BottomLeftColMajor")==0)
    {
        _dataorder=DataOrder::BottomLeftColMajor;
    }
    else if (dataOrder.compare("BottomLeftRowMajor")==0)
    {
        _dataorder=DataOrder::BottomLeftRowMajor;
    }
    else if (dataOrder.compare("BottomRightColMajor")==0)
    {
        _dataorder=DataOrder::BottomRightColMajor;
    }
    else if (dataOrder.compare("BottomRightRowMajor")==0)
    {
        _dataorder=DataOrder::BottomRightRowMajor;
    }
    else
    {
        throw std::runtime_error("Detector class: Data ordering mode not valid, can not build detector");
    }
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
    if (event.getParent() != this) {
        throw std::runtime_error("Trying to assign DetectorEvent to a different detector");
    }
    Eigen::Vector3d v = getPos(event.getX(), event.getY());
    // No gonio and no values set
    if (!_gonio) {
        if (event.getValues().size())
            throw std::runtime_error("Trying to assign a DetectorEvent with values to a Component with no Goniometer");
        else
            return v;
    }
    else if (_gonio->getNPhysicalAxes()!=event.getValues().size()) {
        throw std::runtime_error("Trying to assign a DetectorEvent with wrong number of values");
    }
    _gonio->transformInPlace(v,event.getValues());
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

bool Detector::receiveKf(double& px, double& py, const Eigen::Vector3d& kf, const Eigen::Vector3d& from, double& t, const std::vector<double>& goniovalues)
{

    if (_gonio)
    {
        Eigen::Vector3d fromt=_gonio->transformInverse(from,goniovalues);
        Eigen::Vector3d kft=_gonio->getInverseHomMatrix(goniovalues).rotation()*kf;
        return hasKf(kft,fromt,px,py,t);
    }
    else
        return hasKf(kf,from,px,py,t);
}

Eigen::Vector3d Detector::getQ(double px, double py,double wave,const std::vector<double>& values,const Eigen::Vector3d& from) const
{
    if (wave<=0)
        throw std::runtime_error("Detector:getQ incident wavelength error, must be >0");
    Eigen::Vector3d q=getKf(px,py,wave,values,from);
    q[1]-=1.0/wave; // ki along y
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
    if (!_gonio) {
        if (values.size()) {
            throw std::runtime_error("Trying to create a DetectorEvent with Goniometer values whilst no gonio is set");
        }
    }
    else if (values.size()!=_gonio->getNPhysicalAxes()) {
        throw std::runtime_error("Trying to create a DetectorEvent with invalid number of Goniometer Axes");
    }

    DetectorEvent result(this, x, y, values);
    return result;
}

} // End namespace Instrument
} // End namespace SX
