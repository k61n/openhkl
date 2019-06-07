#include "Source.h"
#include "Monochromator.h"
#include "Units.h"

namespace nsx {

Source* Source::create(const YAML::Node& node)
{
    // Fetch the source from the factory
    Source* source = new Source(node);
    return source;
}

Source::Source() : Component("source"), _monochromators(), _selectedMonochromator(0) {}

Source::Source(const Source& other)
    : Component(other)
    , _monochromators(other._monochromators)
    , _selectedMonochromator(other._selectedMonochromator)
{
}

Source::Source(const std::string& name)
    : Component(name), _monochromators(), _selectedMonochromator(0)
{
}

Source::Source(const YAML::Node& node) : Component(node), _selectedMonochromator(0)
{
    // Loop over the "monochromator" nodes and add the corresponding pointer to
    // Monochromator objects to the Source
    for (const auto& subnode : node) {
        std::string subnodeName = subnode.first.as<std::string>();
        if (subnodeName.compare("monochromator") == 0) {
            Monochromator m(subnode.second);
            addMonochromator(m);
        }
    }
}

Source* Source::clone() const
{
    return new Source(*this);
}

Source::~Source() {}

Source& Source::operator=(const Source& other)
{
    if (this != &other) {
        Component::operator=(other);
        _selectedMonochromator = other._selectedMonochromator;
        _monochromators = other._monochromators;
    }
    return *this;
}

const std::vector<Monochromator>& Source::monochromators() const
{
    return _monochromators;
}

int Source::nMonochromators() const
{
    return _monochromators.size();
}

void Source::setSelectedMonochromator(size_t i)
{
    if (i < _monochromators.size()) {
        _selectedMonochromator = i;
    } else {
        throw std::runtime_error("setSelectedMonochromator(): index i is out of range");
    }
}

Monochromator& Source::selectedMonochromator()
{
    if (_selectedMonochromator < _monochromators.size()) {
        return _monochromators[_selectedMonochromator];
    } else {
        throw std::runtime_error(
            "getSelectedMonochromator(): selected monochromator does not exist");
    }
}

const Monochromator& Source::selectedMonochromator() const
{
    if (_selectedMonochromator < _monochromators.size()) {
        return _monochromators[_selectedMonochromator];
    } else {
        throw std::runtime_error(
            "getSelectedMonochromator(): selected monochromator does not exist");
    }
}

void Source::addMonochromator(Monochromator mono)
{
    _monochromators.push_back(mono);
}

} // end namespace nsx
