#include <stdexcept>
#include <string>

#include "AxisFactory.h"
#include "RotAxis.h"
#include "TransAxis.h"

namespace nsx {

AxisFactory::AxisFactory() {
  registerCallback("rotation", &RotAxis::create);
  registerCallback("translation", &TransAxis::create);
}

AxisFactory::~AxisFactory() {}

} // end namespace nsx
