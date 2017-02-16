#include <iostream>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include "Component.h"
#include "Gonio.h"

namespace SX
{

namespace Instrument
{

Component::Component() : _name(""), _gonio(), _position(Eigen::Vector3d::Zero())
{
}

Component::Component(const std::string& name) : _name(name), _gonio(), _position(Eigen::Vector3d::Zero())
{
}

Component::Component(const Component& other) : _name(other._name), _gonio(other._gonio), _position(other._position)
{
}

Component::Component(const proptree::ptree& node)
{
    // Set the component name
    _name=node.get<std::string>("name","");

    // Set the component goniometer
    boost::optional<const proptree::ptree&> goniometerNode=node.get_child_optional("goniometer");
    if (!goniometerNode)
        _gonio=nullptr;
    else
    {
        _gonio = std::shared_ptr<Gonio>(new Gonio(goniometerNode.get()));
    }

    // Set the component position
    boost::optional<const proptree::ptree&> positionNode=node.get_child_optional("position");
    if (!positionNode)
        _position=Eigen::Vector3d::Zero();
    else
    {
        double x=positionNode.get().get<double>("x");
        double y=positionNode.get().get<double>("y");
        double z=positionNode.get().get<double>("z");
        _position=Eigen::Vector3d(x,y,z);
    }
}

Component::~Component()
{
}

Component& Component::operator=(const Component& other)
{
    if (this != &other) {
        _name = other._name;
        _gonio = other._gonio;
        _position = other._position;
    }
    return *this;
}

std::shared_ptr<Gonio> Component::getGonio() const
{
    return _gonio;
}

const std::string& Component::getName() const
{
    return _name;
}

Eigen::Vector3d Component::getPosition(const std::vector<double>& goniosetup) const
{
    if (_gonio.get()==nullptr) {
        return _position;
    }
     return _gonio->transform(_position,goniosetup);
}

//Eigen::Vector3d Component::getPosition(const ComponentState& state) const
//{
//	if (_gonio.get()==nullptr)
//		return _position;
//	else
//		return _gonio->transform(_position,state._values);
//}

const Eigen::Vector3d& Component::getRestPosition() const
{
    return _position;
}

void Component::setGonio(std::shared_ptr<Gonio> gonio)
{
    if (gonio.get()!=nullptr)
        _gonio=gonio;
}

void Component::setName(const std::string& name)
{
    _name = name;
}

void Component::setRestPosition(const Vector3d& v)
{
    _position=v;
}

bool Component::hasGonio() const
{
    return (_gonio.get()!=nullptr);
}

ComponentState Component::createState()
{
    ComponentState state;
    state._ptrComp=this;
    if (hasGonio()) {
        state._values.resize(_gonio->getNPhysicalAxes(),0);
    }
    return state;
}

ComponentState Component::createState(const std::vector<double>& values)
{
    ComponentState state;
    state._ptrComp = this;
    if (hasGonio()) {
        if (values.size()!=_gonio->getNPhysicalAxes()) {
            throw std::runtime_error("Trying to create a state from component "+_name+" with wrong number of Goniometer values");
        }
        state._values=values;
    }
    return state;
}

ComponentState Component::createStateFromEigen(const Eigen::VectorXd& values)
{
    ComponentState state;
    state._ptrComp=this;
    unsigned int nValues=static_cast<unsigned int>(values.size());
    if (hasGonio())  {
        if (nValues!=_gonio->getNPhysicalAxes()) {
            throw std::runtime_error("Trying to create a state from component "+_name+" with wrong number of Goniometer values");
        }
        state._values.resize(nValues);
        memcpy(state._values.data(),values.data(),values.size()*sizeof(double));
    }
    return state;
}

ComponentState Component::createState(const std::map<std::string,double>& values)
{
    ComponentState state;
    state._ptrComp=this;
    if (hasGonio()) {
        std::vector<double> v(_gonio->getNPhysicalAxes());
        std::size_t comp=0;
        for (auto a : _gonio->getAxes()) {
            if (!a->isPhysical())
                continue;
            auto it=values.find(a->getLabel());
            v[comp++] = (it != values.end()) ? it->second : 0.0;
        }
        state._values=v;
    }
    return state;
}

std::size_t Component::getNAxes() const
{
    if (hasGonio()) {
        return _gonio->getNAxes();
    }
    return 0;
}

std::size_t Component::getNPhysicalAxes() const
{
    if (hasGonio()) {
        return _gonio->getNPhysicalAxes();
    }
    return 0;
}

std::map<unsigned int,std::string> Component::getPhysicalAxesNames() const
{
    std::map<unsigned int,std::string> names;

    if (_gonio) {
        for (auto a : _gonio->getAxes()) {
            if (a->isPhysical()) {
                names.insert(std::pair<unsigned int,std::string>(a->getId(),a->getLabel()));
            }
        }
    }
    return names;
}

std::vector<unsigned int> Component::getPhysicalAxesIds() const
{
    std::vector<unsigned int> ids;

    if (_gonio) {
        ids.reserve(_gonio->getNPhysicalAxes());
        for (auto a : _gonio->getAxes()) {
            if (a->isPhysical()) {
                ids.push_back(a->getId());
            }
        }
    }
    return ids;
}

} // namespace Instrument
} // namespace SX
