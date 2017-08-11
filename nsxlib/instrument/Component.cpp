#include <iostream>

#include "Component.h"
#include "Gonio.h"
#include "../utils/YAMLType.h"

namespace nsx {

Component::Component() : _name(""), _gonio(), _position(Eigen::Vector3d::Zero())
{
}

Component::Component(const std::string& name) : _name(name), _gonio(), _position(Eigen::Vector3d::Zero())
{
}

Component::Component(const Component& other) : _name(other._name), _gonio(other._gonio), _position(other._position)
{
}

Component::Component(const YAML::Node& node)
{
    // Set the component name
    _name = node["name"].as<std::string>();

    _gonio = node["goniometer"] ? std::make_shared<Gonio>(Gonio(node["goniometer"])) : nullptr;

    _position = node["position"] ? node["position"].as<Eigen::Vector3d>() : Eigen::Vector3d::Zero();
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

sptrGonio Component::getGonio() const
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

void Component::setGonio(sptrGonio gonio)
{
    _gonio = gonio;
}

void Component::setName(const std::string& name)
{
    _name = name;
}

void Component::setRestPosition(const Eigen::Vector3d& v)
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

} // end namespace nsx
