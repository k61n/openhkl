#include "TransAxis.h"

namespace SX{
namespace Instrument{

TransAxis::TransAxis(const std::string& label, bool physical) : Axis(label,physical)
{
}

TransAxis::TransAxis(const std::string& label, const Vector3d& axis, bool physical) : Axis(label,physical)
{
	_axis = axis;
	_axis.normalize();
}

TransAxis::TransAxis(const TransAxis& other) : Axis(other)
{
	_axis.normalize();
}

TransAxis& TransAxis::operator=(const TransAxis& other)
{
	if (this != &other)
	{
		Axis::operator=(other);
	}
	return *this;
}

TransAxis::~TransAxis()
{
}

Eigen::Transform<double,3,Eigen::Affine> TransAxis::getHomMatrix(double value) const
{
	Transform<double,3,Eigen::Affine> mat=Transform<double,3,Eigen::Affine>::Identity();
	mat.translation()=_axis*(value+_offset);
	return mat;
}



}
}
