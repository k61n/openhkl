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

Component* ComponentState::getParent() const
{
	return _ptrComp;
}

const std::vector<double>& ComponentState::getValues() const
{
	return _values;
}


} // end namespace Instrument
} // End namespace SX
