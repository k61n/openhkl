#include "Peak3D.h"
#include "PFactorPeakValidator.h"

namespace nsx {

PeakValidator* PFactorPeakValidator::create(const std::map<std::string,double>& parameters)
{
    return new PFactorPeakValidator(parameters);
}

PFactorPeakValidator::PFactorPeakValidator() : PeakValidator()
{
}

PFactorPeakValidator::PFactorPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
    _parameters["p-value"] = 0.8;
}

bool PFactorPeakValidator::isValid(sptrPeak3D peak) const
{
    return peak->pValue() < _parameters.at("p-value");
}

std::string PFactorPeakValidator::description() const
{
    return "Filter peaks statistically not distinguishable from background";
}

void PFactorPeakValidator::setParameters(const std::map<std::string,double>& parameters)
{
    auto it = parameters.find("p-value");
    if (it != parameters.end()) {
        _parameters["p-value"] = it->second;
    }
}

} // end namespace nsx
