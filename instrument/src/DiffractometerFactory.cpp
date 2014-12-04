#include "DiffractometerFactory.h"
#include "D9FourCircles.h"
#include "D10FourCircles.h"
#include "D19FourCircles.h"
#include "Cyclops.h"

namespace SX
{

namespace Instrument
{

DiffractometerFactory::DiffractometerFactory()
{
	registerCallback("D9 4-circles" ,&D9FourCircles::create);
	registerCallback("D10 4-circles",&D10FourCircles::create);
	registerCallback("D19 4-circles",&D19FourCircles::create);
	registerCallback("Cyclops",&Cyclops::create);
}

DiffractometerFactory::~DiffractometerFactory() {
}

} // end namespace Instrument

} // end namespace SX
