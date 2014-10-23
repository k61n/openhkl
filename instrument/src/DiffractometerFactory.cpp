#include "DiffractometerFactory.h"
#include "D9FourCircles.h"
#include "D10FourCircles.h"
#include "D19FourCircles.h"

namespace SX
{

namespace Instrument
{

DiffractometerFactory::DiffractometerFactory()
{
	registerCallback("D9-4c",&D9FourCircles::create);
	registerCallback("D10-4c",&D10FourCircles::create);
	registerCallback("D19-4c",&D19FourCircles::create);
}

DiffractometerFactory::~DiffractometerFactory() {
}

} // end namespace Instrument

} // end namespace SX
