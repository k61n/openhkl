#include <iostream>

#include "Component.h"
#include "Gonio.h"
#include "Units.h"
#include "YAMLType.h"

namespace nsx {


Component::Component(const std::string& name)
: _name(name),
  _gonio()
{
}

Component::Component(const YAML::Node& node)
{
    // Set the component name
    _name = node["name"].as<std::string>();

    _gonio = node["goniometer"] ? std::make_shared<Gonio>(Gonio(node["goniometer"])) : nullptr;
}

Component::~Component()
{
}

sptrGonio Component::gonio() const
{
    return _gonio;
}

const std::string& Component::name() const
{
    return _name;
}

void Component::setGonio(sptrGonio gonio)
{
    _gonio = gonio;
}

void Component::setName(const std::string& name)
{
    _name = name;
}

bool Component::hasGonio() const
{
    return _gonio != nullptr;
}

} // end namespace nsx
