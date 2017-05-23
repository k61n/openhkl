#include <limits>
#include <stdexcept>

#include "Axis.h"
#include "AxisFactory.h"

namespace nsx {

Axis* Axis::create(const boost::property_tree::ptree& node)
{
	// Create an instance of the source factory
	AxisFactory* axisFactory=AxisFactory::Instance();

	// Get the axis type
	std::string axisType=node.get<std::string>("<xmlattr>.type");

	// Fetch the axis from the factory
	Axis* axis = axisFactory->create(axisType,node);

	return axis;
}

Axis::Axis()
: _label("axis"),
  _axis(Eigen::Vector3d(0.0,0.0,1.0)),
  _offset(0.0),
  _min(-std::numeric_limits<double>::infinity()),
  _max(std::numeric_limits<double>::infinity()),
  _offsetFixed(false),
  _physical(true),
  _id(0)
{
}

Axis::Axis(const std::string& label)
: _label(label),
  _axis(Eigen::Vector3d(0.0,0.0,1.0)),
  _offset(0.0),
  _min(-std::numeric_limits<double>::infinity()),
  _max(std::numeric_limits<double>::infinity()),
  _offsetFixed(false),
  _physical(true),
  _id(0)
{
}

Axis::Axis(const std::string& label, const Eigen::Vector3d& axis)
: _label(label),
  _offset(0.0),
  _min(-std::numeric_limits<double>::infinity()),
  _max(std::numeric_limits<double>::infinity()),
  _offsetFixed(false),
  _physical(true),
  _id(0)
{
	setAxis(axis);
}

Axis::Axis(const Axis& other)
: _label(other._label),
  _axis(other._axis),
  _offset(other._offset),
  _min(other._min),
  _max(other._max),
  _offsetFixed(other._offsetFixed),
  _physical(other._physical),
  _id(other._id)
{
}

Axis::Axis(const boost::property_tree::ptree& node)
{
	_label=node.get<std::string>("name");

	const auto& axisDirectionNode=node.get_child("direction");
	double nx=axisDirectionNode.get<double>("x");
	double ny=axisDirectionNode.get<double>("y");
	double nz=axisDirectionNode.get<double>("z");

	Eigen::Vector3d axis(nx,ny,nz);
	axis.normalize();

	_axis=axis;

	_offset=node.get<double>("offset",0.0);
	_offsetFixed=false;

	_min=node.get<double>("min",-std::numeric_limits<double>::infinity());
	_max=node.get<double>("max", std::numeric_limits<double>::infinity());
	_physical=node.get<bool>("physical");

	_id=node.get<unsigned int>("id",0);
}

Axis& Axis::operator=(const Axis& other)
{
	if (this != &other)
	{
		_label       = other._label;
		_axis        = other._axis;
		_offset      = other._offset;
		_min         = other._min;
		_max         = other._max;
		_offsetFixed = other._offsetFixed;
		_physical    = other._physical;
		_id          = other._id;
	}
	return *this;
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

void Axis::setAxis(const Eigen::Vector3d& axis)
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

void Axis::setId(unsigned int id)
{
	_id = id;
}

unsigned int Axis::getId() const
{
	return _id;
}

void Axis::setOffsetFixed(bool fixed)
{
	_offsetFixed=fixed;
}
bool Axis::hasOffsetFixed() const
{
	return _offsetFixed;
}
void Axis::setOffset(double offset,bool override)
{
	if (!_offsetFixed || override)
		_offset=offset;
}
void Axis::addOffset(double offset)
{
	if (!_offsetFixed)
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

Eigen::Vector3d Axis::transform(const Eigen::Vector3d& v,double value)
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

void Axis::setPhysical(bool physical)
{
	_physical = physical;
}

bool Axis::isPhysical() const
{
	return _physical;
}

} // end namespace nsx

