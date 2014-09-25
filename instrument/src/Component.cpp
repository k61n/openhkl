#include <iostream>
#include <boost/foreach.hpp>
#include "Component.h"
#include "Gonio.h"


namespace SX
{

namespace Instrument
{

Component::Component():_name(""),_gonio(),_position(Eigen::Vector3d::Zero())
{
}

Component::Component(const std::string& name) : _name(name), _gonio(), _position(Eigen::Vector3d::Zero())
{
}

Component::~Component()
{
}

const std::string& Component::getName() const
{
	return _name;
}

void Component::setGonio(std::shared_ptr<Gonio> gonio)
{
	if (gonio.get()!=nullptr)
		_gonio=gonio;
}

std::shared_ptr<Gonio> Component::getGonio() const
{
	return _gonio;
}

void Component::setRestPosition(const Vector3d& v)
{
	_position=v;
}

const Eigen::Vector3d& Component::getRestPosition() const
{
	return _position;
}

Eigen::Vector3d Component::getPosition(const std::vector<double>& goniosetup) const
{
	if (_gonio.get()==nullptr)
		return _position;
	else
		return _gonio->transform(_position,goniosetup);
}

Eigen::Vector3d Component::getPosition(const ComponentState& state) const
{
	if (_gonio.get()==nullptr)
		return _position;

		return _gonio->transform(_position,state._values);
}

bool Component::hasGonio() const
{
	return (_gonio.get()!=nullptr);
}

ComponentState Component::createState(const std::vector<double>& values)
{
	ComponentState state;
	state._ptrComp=this;
	if (values.size()!=_gonio->numberOfAxes())
		throw std::runtime_error("Trying to create a state from component "+_name+" with wrong number of Goniometer values");
	state._values=values;
	return state;
}

void Component::parse(const ptree& node)
{
	// A component must have a name in its XML node.
	_name=node.get<std::string>("name");

	// A component may have a x, y and z position in their corresponding XML node. If not, their position is set to the origin.
	_position[0] = node.get<double>("position.<xmlattr>.x",0.0);
	_position[1] = node.get<double>("position.<xmlattr>.y",0.0);
	_position[2] = node.get<double>("position.<xmlattr>.z",0.0);

	// Loop over the inner nodes to detect any "modifier" node. There must at most one such node.
//	BOOST_FOREACH(ptree::value_type v, node)
//	{
//		if (v.first == "modifier")
//		{
//			if (_gonio.get() == nullptr)
//				_gonio=new Gonio(v.second);
//		}
//	}
//
//	// Parse the XML nodes that are specific to the components.
//	_parse(node);
}

std::size_t Component::numberOfAxes() const
{
	if (!_gonio)
		return 0;
	return _gonio->numberOfAxes();
}

} // end namespace Instrument

} // end namespace SX

