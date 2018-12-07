#include <cmath>
#include <iostream>

#include "RotAxis.h"

namespace nsx {

Axis* RotAxis::create(const YAML::Node& node)
{
	return new RotAxis(node);
}

RotAxis::RotAxis() : Axis("rotation"), _dir(Direction::CCW)
{
}

RotAxis::RotAxis(const std::string& label) : Axis(label),_dir(Direction::CCW)
{
}

RotAxis::RotAxis(const std::string& label, const Eigen::Vector3d& axis, Direction direction) : Axis(label,axis), _dir(direction)
{
}

RotAxis::RotAxis(const RotAxis& other) : Axis(other), _dir(other._dir)
{
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

RotAxis::RotAxis(const YAML::Node& node) : Axis(node)
{
	bool clockwise = node["clockwise"].as<bool>();
	_dir = clockwise ? Direction::CW : Direction::CCW;
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
	_dir = dir;
}

RotAxis::Direction RotAxis::rotationDirection() const
{
	return _dir;
}

Eigen::Matrix3d RotAxis::rotationMatrix(double angle) const
{
	Eigen::Quaterniond temp = quaternion(angle);
	return temp.toRotationMatrix();
}
Eigen::Transform<double,3,Eigen::Affine> RotAxis::affineMatrix(double angle) const
{
	Eigen::Transform<double,3,Eigen::Affine> hom=Eigen::Transform<double,3,Eigen::Affine>::Identity();
	Eigen::Quaterniond temp = quaternion(angle);
	hom.linear()=temp.toRotationMatrix();
	return hom;
}

Eigen::Quaterniond RotAxis::quaternion(double angle) const
{
	if (_dir==Direction::CW)
		angle*=-1;
	// Create the quaternion representing this rotation
	double hc = cos(0.5*angle);
	double hs = sin(0.5*angle);
	Eigen::Quaterniond temp(hc,_axis(0)*hs,_axis(1)*hs,_axis(2)*hs);
	return temp;
}

std::ostream& RotAxis::printSelf(std::ostream& os) const
{
    os << "Rotation Axis: " << _axis.transpose() << ", orientation: ";
    os << (_dir == Direction::CW ? "clockwise" : "counter-clockwise");

    return os;
}

} // end namespace nsx

