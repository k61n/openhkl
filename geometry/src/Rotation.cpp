#include "Rotation.h"

namespace SX
{

namespace Geometry
{

Rotation::Rotation() : _axis(Vector3d(0,0,1)) , _dir(CCW) ,_offset(0.0)
{
}

Rotation::Rotation(const Vector3d& axis, Direction direction, double offset) : _axis(axis) , _dir(direction) , _offset(offset)
{
	_axis.normalize();
}

Rotation::~Rotation()
{
}

const Vector3d& Rotation::getAxis() const
{
	return _axis;
}

Vector3d& Rotation::getAxis()
{
	return _axis;
}

double Rotation::getOffset() const
{
	return _offset;
}

Rotation::Direction Rotation::getRotationDirection() const
{
	return _dir;
}

Matrix3d Rotation::getRotation(double angle) const
{
	// Create a quaternion from the input angle and rotation axis.
    Eigen::Quaternion<double> quat(Eigen::AngleAxis<double>(angle,_axis));

	return quat.toRotationMatrix();
}

Vector3d Rotation::getTranslation(double angle) const
{
	return Vector3d(0,0,0);
}

HomMatrix Rotation::getTransformation(double angle) const
{
	// Create and initialize an Eigen transformation (homogeneous) matrix.
	HomMatrix trans;
	trans.setIdentity();

	// Create a quaternion from the input angle and rotation axis.
    Eigen::Quaternion<double> quat(Eigen::AngleAxis<double>(angle,_axis));

    // Set the rotation part of the homogeneous matrix with the quaternion-based matrix.
    trans.rotation()=quat.toRotationMatrix();

	return trans;
}

void Rotation::setAxis(const Vector3d& v)
{
	_axis=v;
	// Normalize the axis in case it was not.
	_axis.normalize();
}

void Rotation::setOffset(double offset)
{
	_offset=offset;
}

void Rotation::setRotationDirection(Direction dir)
{
	_dir=dir;
}

} // end namespace Geometry

} // end namespace SX
