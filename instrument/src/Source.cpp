#include "Source.h"

namespace SX
{

namespace Instrument
{

Source::Source() : Component()
{
}

Source::Source(const Source& other) : Component(other)
{
}

Source::Source(const std::string& name): Component(name)
{
}

Source::~Source()
{
}

Source& Source::operator=(const Source& other)
{
	if (this != &other)
	{
		Component::operator=(other);
	}
	return *this;
}

} // end namespace Instrument

} // end namespace SX
