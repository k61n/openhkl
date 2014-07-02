#include <iostream>

#include "Rotation.h"

namespace SX
{

namespace Geometry
{

PrimitiveTransformation* Rotation::Create(const ptree& node)
{
	return new Rotation(node);
}

Rotation::Rotation() : PrimitiveTransformation(), _dir(Rotation::Direction::CCW)
{
}

Rotation::Rotation(const Vector3d& axis, Rotation::Direction dir, double offset) : PrimitiveTransformation(axis,offset), _dir(dir)
{
}

Rotation::Rotation(const ptree& node)
{
	parse(node);
}

Rotation::~Rotation()
{
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
    trans.linear() = quat.toRotationMatrix();

	return trans;
}

void Rotation::_parse(const ptree& node)
{
	std::cout<<"I PARSE A ROTATION NODE"<<std::endl;
}

void Rotation::setRotationDirection(Rotation::Direction dir)
{
	_dir=dir;
}

} // end namespace Geometry

} // end namespace SX
