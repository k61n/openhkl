#include <iostream>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include "Component.h"
#include "Gonio.h"

namespace nsx {
namespace Instrument {

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
    _name = node.get<std::string>("name","");

    // Set the component goniometer
    boost::optional<const proptree::ptree&> goniometerNode = node.get_child_optional("goniometer");
    if (!goniometerNode) {
        _gonio = nullptr;
    } else {
        _gonio = std::shared_ptr<Gonio>(new Gonio(goniometerNode.get()));
    }

    // Set the component position
    boost::optional<const proptree::ptree&> positionNode = node.get_child_optional("position");
    if (!positionNode) {
        _position = Eigen::Vector3d::Zero();
    } else {
        double x = positionNode.get().get<double>("x");
        double y = positionNode.get().get<double>("y");
        double z = positionNode.get().get<double>("z");
        _position = Eigen::Vector3d(x,y,z);
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
    if (_gonio.get() == nullptr) {
        return _position;
    }
    return _gonio->transform(_position, goniosetup);
}

const Eigen::Vector3d& Component::getRestPosition() const
{
    return _position;
}

void Component::setGonio(std::shared_ptr<Gonio> gonio)
{
    _gonio = gonio;
}

void Component::setName(const std::string& name)
{
    _name = name;
}

void Component::setRestPosition(const Vector3d& v)
{
    _position = v;
}

bool Component::hasGonio() const
{
    return _gonio != nullptr;
}

ComponentState Component::createState(const std::map<std::string,double>& values)
{
    std::vector<double> values_vec;

    if (hasGonio()) {
        values_vec.resize(_gonio->getNPhysicalAxes(), 0);
        std::size_t comp = 0;
        for (auto&& a: _gonio->getAxes()) {
            if (!a->isPhysical()) {
                continue;
            }
            auto it = values.find(a->getLabel());
            values_vec[comp++] = (it != values.end()) ? it->second : 0.0;
        }
    }
    return ComponentState(this, values_vec);
}

} // namespace Instrument
} // namespace nsx
