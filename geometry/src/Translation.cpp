#include <iostream>

#include "Translation.h"

namespace SX
{

namespace Geometry
{

PrimitiveTransformation* Translation::Create(const ptree& node)
{
	return new Translation(node);
}

Translation::Translation() : PrimitiveTransformation()
{
}

Translation::Translation(const Vector3d& axis, double offset) : PrimitiveTransformation(axis,offset)
{
}

Translation::Translation(const ptree& node)
{
	parse(node);
}

Translation::~Translation()
{
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

void Translation::_parse(const ptree& node)
{
}

} // end namespace Geometry

} // end namespace SX
