#include "TransAxis.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

Axis* TransAxis::create(const proptree::ptree& node)
{
	return new TransAxis(node);
}

TransAxis::TransAxis() : Axis("translation")
{
	_units = "mm";
}

TransAxis::TransAxis(const std::string& label) : Axis(label)
{
	_units = "mm";
}

TransAxis::TransAxis(const std::string& label, const Vector3d& axis) : Axis(label,axis)
{
	_units = "mm";
}

TransAxis::TransAxis(const TransAxis& other) : Axis(other)
{
}

TransAxis::TransAxis(const proptree::ptree& node) : Axis(node)
{
	Units::UnitsManager* um=SX::Units::UnitsManager::Instance();
	_units=node.get<std::string>("<xmlattr>.units","mm");
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
