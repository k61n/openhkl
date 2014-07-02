#include <iostream>

#include "PrimitiveTransformation.h"

namespace SX
{

namespace Geometry
{

PrimitiveTransformation::PrimitiveTransformation() : XMLConfigurable(), _axis(Vector3d(0,0,1)), _offset(0.0)
{
}

PrimitiveTransformation::PrimitiveTransformation(const Vector3d& axis, double offset) : XMLConfigurable(), _axis(axis), _offset(offset)
{
	_axis.normalize();
}

PrimitiveTransformation::~PrimitiveTransformation()
{
}

const Vector3d& PrimitiveTransformation::getAxis() const
{
	return _axis;
}

Vector3d& PrimitiveTransformation::getAxis()
{
	return _axis;
}

double PrimitiveTransformation::getOffset() const
{
	return _offset;
}

void PrimitiveTransformation::parse(const ptree& node)
{

	// A transformation must have a name in its XML node.
	_name=node.get<std::string>("name");

	// A transformation must have an axis tag in its XML node.
	_axis[0] = node.get<double>("axis.<xmlattr>.x");
	_axis[1] = node.get<double>("axis.<xmlattr>.y");
	_axis[2] = node.get<double>("axis.<xmlattr>.z");

	_parse(node);
}

void PrimitiveTransformation::setAxis(const Vector3d& v)
{
	_axis=v;
	// Normalize the axis in case it was not.
	_axis.normalize();
}

void PrimitiveTransformation::setOffset(double offset)
{
	_offset=offset;
}

} // end namespace Geometry

} // end namespace SX
