#include <Eigen/Dense>

#include "CylindricalDetector.h"
#include "DirectVector.h"
#include "Gonio.h"
#include "ReciprocalVector.h"
#include "RotAxis.h"
#include "TransAxis.h"
#include "Units.h"

namespace nsx {

Detector* CylindricalDetector::create(const YAML::Node& node)
{
    return new CylindricalDetector(node);
}

CylindricalDetector::CylindricalDetector(const std::string& name) : Detector(name)
{
}

CylindricalDetector::CylindricalDetector(const YAML::Node& node) : Detector(node)
{
    UnitsManager* um=UnitsManager::Instance();

    // Set the detector angular width from the property tree node
    auto&& angularWidthNode = node["angular_width"];
    double units = um->get(angularWidthNode["units"].as<std::string>());
    double angularWidth = angularWidthNode["value"].as<double>();
    angularWidth *= units;
    setAngularWidth(angularWidth);

    // Set the detector height from the property tree node
    auto&& heightNode = node["height"];
    units = um->get(heightNode["units"].as<std::string>());
    double height = heightNode["value"].as<double>();
    height *= units;
    setHeight(height);
}

CylindricalDetector::~CylindricalDetector()
{
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

DirectVector CylindricalDetector::pixelPosition(double px, double py) const
{
    if (_nCols==0 || _nRows==0)
        throw std::runtime_error("Detector: number of rows or cols must >0");

    if (_height==0 || _width==0)
        throw std::runtime_error("Detector: width or height must be >0");

    if (_distance==0)
        throw std::runtime_error("Detector: distance must be >0");

    // The coordinates are defined relative to the detector origin
    double x=px-_minCol;
    double y=py-_minRow;

    double gamma=(x/(_nCols-1.0)-0.5)*_angularWidth;
    Eigen::Vector3d result;
    result[0]=_distance*sin(gamma);
    // Angle
    result[1]=_distance*cos(gamma);
    // take the center of the bin
    result[2]=(y/(_nRows-1.0)-0.5)*_height;
    return DirectVector(result);
}

DetectorEvent CylindricalDetector::constructEvent(const DirectVector& from, const ReciprocalVector& kf) const
{
    const DetectorEvent no_event = {0, 0, -1, -1};
    double px, py, tof;

    const Eigen::Vector3d direction = kf.rowVector().transpose();

    // Need to solve equation of the typr (from_xy + f_xy*t)^2=R^2
    double b=2*(from[0]*direction[0]+from[1]*direction[1]);
    double a=(direction[0]*direction[0]+direction[1]*direction[1]);
    double c= from[0]*from[0]+from[1]*from[1]-_distance*_distance;

    double Delta=b*b-4*a*c;
    if (Delta<0)
        return no_event;

    Delta=sqrt(Delta);

    tof=0.5*(-b+Delta)/a;
    if (tof <= 0)
        return no_event;

    Eigen::RowVector3d v = from.vector() + direction*tof;

    double phi=atan2(v[0],v[1])+0.5*_angularWidth;
    if (phi<0 || phi>=_angularWidth)
        return no_event;

    double d=v[2]/_height+0.5;

    if (d<0 || d>1.0)
        return no_event;

    px=phi/_angularWidth*(_nCols-1);
    py=d*(_nRows-1);

    return {px, py, 0.0, tof};
}

Eigen::Matrix3d CylindricalDetector::jacobian(double px, double py) const
{
    Eigen::Matrix3d J;
    J.setZero();
 
    if (_nCols==0 || _nRows==0)
        throw std::runtime_error("Detector: number of rows or cols must >0");

    if (_height==0 || _width==0)
        throw std::runtime_error("Detector: width or height must be >0");

    if (_distance==0)
        throw std::runtime_error("Detector: distance must be >0");

    // The coordinates are defined relative to the detector origin
    double x = px-_minCol;
    double gamma = (x/(_nCols-1.0)-0.5)*_angularWidth;
    double dgamma = 1.0/(_nCols-1.0)*_angularWidth;

    // derivative with respect to px
    J(0,0) = _distance*cos(gamma)*dgamma;
    J(1,0) = -_distance*sin(gamma)*dgamma;

    // derivative with resepct to py
    J(2,1) = 1.0/(_nRows-1.0)*_height;

    return J;
}

} // end namespace nsx

