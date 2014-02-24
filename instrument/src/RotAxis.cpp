#include "Units.h"
#include "RotAxis.h"


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

} // End namespace Geometry

} // End namespace SX

