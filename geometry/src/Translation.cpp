#include "Translation.h"

namespace SX
{

namespace Geometry
{

Translation::Translation() : _axis(Vector3d(0,0,1)) ,_offset(0.0)
{
}

Translation::Translation(const Vector3d& axis, double offset) : _axis(axis), _offset(offset)
{
	_axis.normalize();
}

Translation::~Translation()
{
}

const Vector3d& Translation::getAxis() const
{
	return _axis;
}

Vector3d& Translation::getAxis()
{
	return _axis;
}

double Translation::getOffset() const
{
	return _offset;
}

Matrix3d Translation::getRotation(double angle) const
{
	return Matrix3d::Zero();
}

Vector3d Translation::getTranslation(double angle) const
{
	return angle*_axis;
}

HomMatrix Translation::getTransformation(double angle) const
{
	// Create and initialize an Eigen transformation (homogeneous) matrix.
	HomMatrix trans;
	trans.setIdentity();

    // Set the rotation part of the homogeneous matrix with the quaternion-based matrix.
    trans.translation()=angle*_axis;

	return trans;
}

void Translation::setAxis(const Vector3d& v)
{
	_axis=v;
	// Normalize the axis in case it was not.
	_axis.normalize();
}

void Translation::setOffset(double offset)
{
	_offset=offset;
}

} // end namespace Geometry

} // end namespace SX
