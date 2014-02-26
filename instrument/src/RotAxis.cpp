#include "Units.h"
#include "RotAxis.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <boost/foreach.hpp>

namespace SX
{
namespace Instrument
{

RotAxis::RotAxis():_axis(Vector3d(0,0,1)),_dir(CCW),_offset(0.0)
{
}


RotAxis::RotAxis(const Vector3d& axis, Direction direction, double offset):_axis(axis),_dir(direction),_offset(offset)
{
	_axis.normalize();
}
RotAxis::~RotAxis()
{
}

void RotAxis::setAxis(const Vector3d& v)
{
	_axis=v;
}
const Vector3d& RotAxis::getAxis() const
{
	return _axis;
}
Vector3d& RotAxis::getAxis()
{
	return _axis;
}
void RotAxis::setRotationDirection(Direction dir)
{
	_dir=dir;
}
Direction RotAxis::getRotationDirection() const
{
	return _dir;
}
void RotAxis::setOffset(double offset)
{
	_offset=offset;
}
double RotAxis::getOffset() const
{
	return _offset;
}
Matrix3d RotAxis::getMatrix(double angle) const
{
	Quaterniond temp=getQuat(angle);
	return temp.toRotationMatrix();
}
Eigen::Transform<double,3,Eigen::Affine> RotAxis::getHomMatrix(double angle) const
{
	Eigen::Transform<double,3,Eigen::Affine> hom;
	Quaterniond temp=getQuat(angle);
	hom.linear()=temp.toRotationMatrix();
	return hom;
}
Quaterniond RotAxis::getQuat(double angle) const
{
	//Apply offset first (offset in the same direction).
	angle+=_offset;
	if (_dir==CW)
		angle*=-1;
	// Create the quaternion representing this rotation
	double hc=cos(0.5*angle);
	double hs=sin(0.5*angle);
	Quaterniond temp(hc,_axis(0)*hs,_axis(1)*hs,_axis(2)*hs);
	return temp;
}
std::ostream& operator<<(std::ostream& os, const RotAxis& Rot)
{
	os << "Rotation Axis: " << Rot._axis << ", direction: ";
	if (Rot._dir==CW)
		os << " CW";
	else
		os << " CCW";
	return os;
}

void RotAxis::readXML(std::istream& is)
{
	using boost::property_tree::ptree;
	ptree pt;
	read_xml(is, pt);

	// traverse pt
	BOOST_FOREACH( ptree::value_type const& v, pt.get_child("Modifier") ) {
		if( v.first == "RotAxis" ) {
			_axis[0]=v.second.get<double>("x");
			_axis[1]=v.second.get<double>("y");
			_axis[2]=v.second.get<double>("z");
			std::string direction=v.second.get<std::string>("Direction");
			std::cout << direction << std::endl;
			if (direction.compare("CCW")==0)
			{
				_dir=CCW;
				std::cout << "CCW";
			}
			else if (direction.compare("CW")==0)
			{
				_dir=CW;
				std::cout << "CW";
			}
		}
	}
	std::cout << _axis;
}

} // End namespace Geometry

} // End namespace SX

