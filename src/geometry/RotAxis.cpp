#include "Constants.h"
#include "RotAxis.h"

namespace SX
{
RotAxis::RotAxis(const V3D& axis, Units::Rotation direction, const char* label):_axis(axis),_dir(direction),_name((label == 0)? " " : label)
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
Matrix33<double> RotAxis::getMatrix(double angle,Units::Angular unit) const
{
	Quat temp(getQuat(angle,unit));
	return temp.toMatrix();
}
Quat RotAxis::getQuat(double angle,Units::Angular unit) const
{
	if (_dir==Units::CW)
		angle*=-1;
	if (unit==Units::Deg)
		angle*=Constants::deg2rad;
	// Create the quaternion representing this rotation
	Quat temp(angle,_axis);
	return temp;
}
std::ostream& operator<<(std::ostream& os, const RotAxis& Rot)
{
	os << "Rotation Axis: " << Rot._name << std::endl;
	os << ", axis: " << Rot._axis << std::endl;
	os << ", direction: ";
	if (Rot._dir==Units::CW)
		os << " CW";
	else
		os << " CCW";
	return os;
}

} // End namespace SX

