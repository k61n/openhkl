#include "Units.h"
#include "RotAxis.h"


using SX::Quat;

namespace SX
{
namespace Geometry
{

RotAxis::RotAxis():_axis(UnitZ),_dir(CCW)
{
}


RotAxis::RotAxis(const V3D& axis, Direction direction):_axis(axis),_dir(direction)
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
void RotAxis::setRotationDirection(Direction dir)
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
	if (_dir==CW)
		angle*=-1;
	// Create the quaternion representing this rotation
	Quat temp(angle,_axis);
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

} // End namespace Geometry

} // End namespace SX

