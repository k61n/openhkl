#include <stdexcept>
#include <string>

#include "MonochromaticSource.h"
#include "PolychromaticSource.h"
#include "SourceFactory.h"

namespace SX
{

namespace Instrument
{

SourceFactory::SourceFactory()
{
	registerCallback("monochromatic",&MonochromaticSource::create);
	registerCallback("polychromatic",&PolychromaticSource::create);
}

SourceFactory::~SourceFactory()
{
}

} // end namespace Instrument

} // end namespace SX
