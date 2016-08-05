#include <stdexcept>
#include <string>

#include "CylindricalDetector.h"
#include "DetectorFactory.h"
#include "FlatDetector.h"
#include "MultiDetector.h"

namespace SX
{

namespace Instrument
{

DetectorFactory::DetectorFactory()
{
	registerCallback("flat" ,&FlatDetector::create);
	registerCallback("cylindrical",&CylindricalDetector::create);
	registerCallback("multidetector",&MultiDetector::create);
}

DetectorFactory::~DetectorFactory()
{
}

} // end namespace Instrument

} // end namespace SX
