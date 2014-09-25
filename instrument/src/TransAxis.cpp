#include "TransAxis.h"

namespace SX{
namespace Instrument{

TransAxis::TransAxis(const std::string& label):Axis(label)
{
}

TransAxis::TransAxis(const std::string& label, const Vector3d& axis):Axis(label)
{
	_axis=axis;
	_axis.normalize();
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
