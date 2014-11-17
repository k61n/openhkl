#include "DetectorFactory.h"

#include "CylindricalDetector.h"
#include "FlatDetector.h"

namespace SX
{

namespace Instrument
{

DetectorFactory::DetectorFactory()
{
	registerCallback("flat" ,&FlatDetector::create);
	registerCallback("cylindrical",&CylindricalDetector::create);
}

DetectorFactory::~DetectorFactory()
{
}

} // end namespace Instrument

} // end namespace SX
