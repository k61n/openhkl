#include "Ellipse.h"

namespace SX
{

namespace Geometry
{

Ellipse::Ellipse()
{
}

Ellipse::Ellipse(const Vector3d& center, const Vector3d& semi_axes, const Vector3d& axis1, const Vector3d& axis2):
_center(center), _semi_axes(semi_axes), _axis1(axis1), _axis2(axis2)
{
}

Ellipse::Ellipse(const Ellipse& ell)
{
	_center    = ell._center;
	_semi_axes = ell._semi_axes;
	_axis1     = ell._axis1;
	_axis2     = ell._axis2;
}

Ellipse& Ellipse::operator=(const Ellipse& ell)
{
	if (this != &ell)
	{
		_center    = ell._center;
		_semi_axes = ell._semi_axes;
		_axis1     = ell._axis1;
		_axis2     = ell._axis2;
	}
	return *this;
}

Ellipse::~Ellipse()
{
}

const Vector3d& Ellipse::getCenter(void) const
{
	return _center;
}

const Vector3d& Ellipse::getSemiAxes(void) const
{
	return _semi_axes;

}

const Vector3d& Ellipse::getAxis1(void) const
{
	return _axis1;
}

const Vector3d& Ellipse::getAxis2(void) const
{
	return _axis2;
}

}

}
