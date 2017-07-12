#include "MaskedPeakValidator.h"
#include "Peak3D.h"

namespace nsx {

bool MaskedPeakValidator::isValid(sptrPeak3D peak) const
{
    return !(peak->isMasked());
}

} // end namespace nsx
