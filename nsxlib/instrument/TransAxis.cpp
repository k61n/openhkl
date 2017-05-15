#include "TransAxis.h"


namespace nsx
{

namespace Instrument
{

Axis* TransAxis::create(const proptree::ptree& node)
{
	return new TransAxis(node);
}

TransAxis::TransAxis() : Axis("translation")
{
}

TransAxis::TransAxis(const std::string& label) : Axis(label)
{
}

TransAxis::TransAxis(const std::string& label, const Vector3d& axis) : Axis(label,axis)
{
}

TransAxis::TransAxis(const TransAxis& other) : Axis(other)
{
}

TransAxis::TransAxis(const proptree::ptree& node) : Axis(node)
{
}

TransAxis::~TransAxis()
{
}

TransAxis* TransAxis::clone() const
{
	return new TransAxis(*this);
}

TransAxis& TransAxis::operator=(const TransAxis& other)
{
	if (this != &other)
		Axis::operator=(other);

	return *this;
}

Eigen::Transform<double,3,Eigen::Affine> TransAxis::getHomMatrix(double value) const
{
	Transform<double,3,Eigen::Affine> mat=Transform<double,3,Eigen::Affine>::Identity();
	mat.translation()=_axis*(value+_offset);
	return mat;
}

}

}
