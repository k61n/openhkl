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

const Component* ComponentState::getParent() const
{
	return _ptrComp;
}


} // end namespace Instrument
} // End namespace SX
