#include "Axis.h"
#include <stdexcept>
#include <iostream>

namespace SX
{
namespace Instrument
{

Axis::Axis(const std::string& label):_label(label),_axis(Eigen::Vector3d::Zero()),_offset(0),_min(0),_max(0)
{
}

Axis::~Axis()
{
}


void Axis::setLabel(const std::string& label)
{
	_label=label;
}

const std::string& Axis::getLabel() const
{
	return _label;
}

void Axis::setAxis(const Vector3d& axis)
{
	if (axis.isZero())
		throw std::runtime_error("Invalid null axis for"+_label);
	_axis=axis;
	_axis.normalize();
}
const Eigen::Vector3d& Axis::getAxis() const
{
	return _axis;
}
void Axis::setOffset(double offset)
{
	_offset=offset;
}
void Axis::addOffset(double offset)
{
	_offset+=offset;
}
double Axis::getOffset() const
{
	return _offset;
}

void Axis::setLimits(double min, double max)
{
	_min=min;
	_max=max;
}
void Axis::setMin(double min)
{
	_min=min;
}
void Axis::setMax(double max)
{
	_max=max;
}
double Axis::getMin() const
{
	return _min;
}
double Axis::getMax() const
{
	return _max;
}

Vector3d Axis::transform(double value,const Vector3d& v)
{
	Eigen::Transform<double,3,Eigen::Affine> hom=getHomMatrix(value);
	return (hom*v.homogeneous());
}


}
}
