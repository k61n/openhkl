#include "Units.h"
#include "RotAxis.h"

namespace SX
{
RotAxis::RotAxis(const V3D& axis, Rotation::Direction direction):_axis(axis),_dir(direction)
{
	_axis.normalize();
}
RotAxis::~RotAxis()
{
}

void RotAxis::setAxis(const V3D& v)
{
	_axis=v;
}
const V3D& RotAxis::getAxis() const
{
	return _axis;
}
V3D& RotAxis::getAxis()
{
	return _axis;
}
void RotAxis::setRotationDirection(Rotation::Direction dir)
{
	_dir=dir;
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
	os << "Rotation Axis: ";
	os << ", axis: " << Rot._axis << std::endl;
	os << ", direction: ";
	if (Rot._dir==Rotation::CW)
		os << " CW";
	else
		os << " CCW";
	return os;
}

} // End namespace SX

