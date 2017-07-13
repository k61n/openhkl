#include "PeakValidator.h"

namespace nsx {

PeakValidator::PeakValidator() : _parameters()
{
}

PeakValidator::PeakValidator(const std::map<std::string,double>& parameters) : _parameters(parameters)
{
}

const std::map<std::string,double>& PeakValidator::parameters() const
{
    return _parameters;
}

} // end namespace nsx
