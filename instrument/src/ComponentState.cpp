#include "ComponentState.h"

namespace SX {
namespace Instrument {

ComponentState::ComponentState():_ptrComp(nullptr),
		_values()
{
}
ComponentState::~ComponentState()
{
}

Component* ComponentState::getParent()
{
	return _ptrComp;
}
void ComponentState::setParent(Component* c)
{
	_ptrComp=c;
}

const std::vector<double>& ComponentState::getValues() const
{
	return _values;
}


} // end namespace Instrument
} // End namespace SX
