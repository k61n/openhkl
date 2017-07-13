#include "Peak3D.h"
#include "PFactorPeakValidator.h"

namespace nsx {

PFactorPeakValidator::PFactorPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
}

bool PFactorPeakValidator::isValid(sptrPeak3D peak) const
{
    return peak->pValue() < _parameters.at("pmax");
}

} // end namespace nsx
