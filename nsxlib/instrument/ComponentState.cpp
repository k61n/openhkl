#include "ComponentState.h"

namespace SX {
namespace Instrument {

ComponentState::ComponentState() : _ptrComp(nullptr), _values()
{
}

ComponentState::ComponentState(const ComponentState& other) : _ptrComp(other._ptrComp), _values(other._values)
{
}

ComponentState::~ComponentState()
{
}

ComponentState& ComponentState::operator=(const ComponentState& other)
{
    if (this != &other) {
        _ptrComp = other._ptrComp;
        _values = other._values;
    }
    return *this;
}

Component* ComponentState::getParent() const
{
    return _ptrComp;
}
void ComponentState::setParent(Component* c)
{
    _ptrComp = c;
}

const std::vector<double>& ComponentState::getValues() const
{
    return _values;
}

} // end namespace Instrument
} // End namespace SX
