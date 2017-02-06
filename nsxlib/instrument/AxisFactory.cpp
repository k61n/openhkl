#include <stdexcept>
#include <string>

#include "AxisFactory.h"
#include "RotAxis.h"
#include "TransAxis.h"

namespace SX {
namespace Instrument {

AxisFactory::AxisFactory()
{
    registerCallback("rotation",&RotAxis::create);
    registerCallback("translation",&TransAxis::create);
}

AxisFactory::~AxisFactory()
{
}

} // end namespace Instrument
} // end namespace SX
