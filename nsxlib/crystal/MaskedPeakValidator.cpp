#include "MaskedPeakValidator.h"
#include "Peak3D.h"

namespace nsx {

MaskedPeakValidator::MaskedPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
}

bool MaskedPeakValidator::isValid(sptrPeak3D peak) const
{
    return !(peak->isMasked());
}

} // end namespace nsx
