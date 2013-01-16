#include "Units.h"
#include "RotAxis.h"

namespace SX
{
RotAxis::RotAxis(const V3D& axis, Rotation::Direction direction, const char* label):_axis(axis),_dir(direction),_name((label == 0)? " " : label)
{
	_axis.normalize();
}
RotAxis::~RotAxis()
{
}

std::string& RotAxis::getLabel()
{
	return _name;
}
const std::string& RotAxis::getLabel() const
{
	return _name;
}
Matrix33<double> RotAxis::getMatrix(double angle) const
{
	Quat temp(getQuat(angle));
	return temp.toMatrix();
}
Quat RotAxis::getQuat(double angle) const
{
	if (_dir==Rotation::CW)
		angle*=-1;
	// Create the quaternion representing this rotation
	Quat temp(angle,_axis);
	return temp;
}
std::ostream& operator<<(std::ostream& os, const RotAxis& Rot)
{
	os << "Rotation Axis: " << Rot._name << std::endl;
	os << ", axis: " << Rot._axis << std::endl;
	os << ", direction: ";
	if (Rot._dir==Rotation::CW)
		os << " CW";
	else
		os << " CCW";
	return os;
}

} // End namespace SX

