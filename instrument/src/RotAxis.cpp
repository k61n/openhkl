#include "Units.h"
#include "RotAxis.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <boost/foreach.hpp>
#include <cmath>

namespace SX
{

namespace Instrument
{

RotAxis::RotAxis() : Axis("rotation"), _dir(CCW)
{
}

RotAxis::RotAxis(const std::string& label) : Axis(label),_dir(CCW)
{
}

RotAxis::RotAxis(const std::string& label, const Vector3d& axis, Direction direction) : Axis(label,axis)
{
	_dir = direction;
}

RotAxis::RotAxis(const RotAxis& other) : Axis(other)
{
	_dir = other._dir;
}

RotAxis& RotAxis::operator=(const RotAxis& other)
{
	if (this != &other)
	{
		Axis::operator=(other);
		_dir = other._dir;
	}
	return *this;
}

RotAxis::~RotAxis()
{
}

RotAxis* RotAxis::clone() const
{
	return new RotAxis(*this);
}

void RotAxis::setRotationDirection(Direction dir)
{
	_dir=dir;
}
RotAxis::Direction RotAxis::getRotationDirection() const
{
	return _dir;
}
Matrix3d RotAxis::getRotationMatrix(double angle) const
{
	Quaterniond temp=getQuat(angle);
	return temp.toRotationMatrix();
}
Eigen::Transform<double,3,Eigen::Affine> RotAxis::getHomMatrix(double angle) const
{
	Eigen::Transform<double,3,Eigen::Affine> hom=Eigen::Transform<double,3,Eigen::Affine>::Identity();
	Quaterniond temp=getQuat(angle);
	hom.linear()=temp.toRotationMatrix();
	return hom;
}

Quaterniond RotAxis::getQuat(double angle) const
{
	//Apply offset first (offset in the same direction).
	angle+=_offset;
	if (_dir==RotAxis::CW)
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
	if (Rot._dir==RotAxis::CW)
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

} // End namespace Instrument

} // End namespace SX

