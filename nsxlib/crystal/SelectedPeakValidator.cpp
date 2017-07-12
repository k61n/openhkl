#include "Peak3D.h"
#include "SelectedPeakValidator.h"

namespace nsx {

bool SelectedPeakValidator::isValid(sptrPeak3D peak) const
{
    return (peak->isSelected());
}

} // end namespace nsx
