#include "Threshold.h"

namespace nsx {

Threshold::Threshold()
: _parameters()
{
}

Threshold::~Threshold()
{
}

std::map<std::string,double>& Threshold::parameters()
{
    return _parameters;
}

const std::map<std::string,double>& Threshold::parameters() const
{
    return _parameters;
}

} // end namespace nsx
