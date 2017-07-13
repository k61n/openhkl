#include "MaskedPeakValidator.h"
#include "Peak3D.h"

namespace nsx {

PeakValidator* MaskedPeakValidator::create(const std::map<std::string,double>& parameters)
{
    return new MaskedPeakValidator(parameters);
}

MaskedPeakValidator::MaskedPeakValidator() : PeakValidator()
{
}

MaskedPeakValidator::MaskedPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
    _parameters.clear();
}

bool MaskedPeakValidator::isValid(sptrPeak3D peak) const
{
    return !(peak->isMasked());
}

std::string MaskedPeakValidator::description() const
{
    return "Filter masked peaks";
}

void MaskedPeakValidator::setParameters(const std::map<std::string,double>& parameters)
{
}

} // end namespace nsx
