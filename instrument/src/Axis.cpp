#include "Axis.h"
#include <stdexcept>
#include <iostream>
#include <limits>

namespace SX
{
namespace Instrument
{

Axis::Axis(const std::string& label):_label(label),_axis(Eigen::Vector3d::Zero()),_offset(0),_min(-std::numeric_limits<double>::infinity()),_max(std::numeric_limits<double>::infinity()),_offsetFixed(false)
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
		throw std::runtime_error("Invalid null axis for axis "+_label);
	_axis=axis;
	// Normalize the axis
	_axis.normalize();
}
const Eigen::Vector3d& Axis::getAxis() const
{
	return _axis;
}

void Axis::setOffsetFixed(bool fixed)
{
	_offsetFixed=fixed;
}
bool Axis::hasOffsetFixed() const
{
	return _offsetFixed;
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
void Axis::setLowLimit(double min)
{
	_min=min;
}
void Axis::setHighLimit(double max)
{
	_max=max;
}
double Axis::getLowLimit() const
{
	return _min;
}
double Axis::getHighLimit() const
{
	return _max;
}

Vector3d Axis::transform(const Vector3d& v,double value)
{
	Eigen::Transform<double,3,Eigen::Affine> hom=getHomMatrix(value);
	return (hom*v.homogeneous());
}

void Axis::checkRange(double value)
{
	if (value<_min || value>_max)
		throw std::range_error("Axis "+_label+": value given for transformation not within limits");
	return;
}

}
}
