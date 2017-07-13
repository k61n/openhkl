#include "Peak3D.h"
#include "SelectedPeakValidator.h"

namespace nsx {

PeakValidator* SelectedPeakValidator::create(const std::map<std::string,double>& parameters)
{
    return new SelectedPeakValidator(parameters);
}

SelectedPeakValidator::SelectedPeakValidator() : PeakValidator()
{
}

SelectedPeakValidator::SelectedPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
    _parameters.clear();
}

bool SelectedPeakValidator::isValid(sptrPeak3D peak) const
{
    return (peak->isSelected());
}

std::string SelectedPeakValidator::description() const
{
    return "Filter unselected peaks";
}

void SelectedPeakValidator::setParameters(const std::map<std::string,double>& parameters)
{
}

} // end namespace nsx
