#include "Peak3D.h"
#include "SelectedPeakValidator.h"

namespace nsx {

SelectedPeakValidator::SelectedPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
}

bool SelectedPeakValidator::isValid(sptrPeak3D peak) const
{
    return (peak->isSelected());
}

} // end namespace nsx
