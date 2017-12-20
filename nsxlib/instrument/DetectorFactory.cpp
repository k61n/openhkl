#include <stdexcept>
#include <string>

#include "CylindricalDetector.h"
#include "DetectorFactory.h"
#include "FlatDetector.h"
#include "MultiDetector.h"

namespace nsx {

DetectorFactory::DetectorFactory()
{
	registerCallback("flat", &FlatDetector::create);
	registerCallback("cylindrical", &CylindricalDetector::create);
}

DetectorFactory::~DetectorFactory()
{
}

} // end namespace nsx

