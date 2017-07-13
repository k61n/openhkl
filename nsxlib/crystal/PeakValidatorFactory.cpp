#include "MaskedPeakValidator.h"
#include "IndexedPeakValidator.h"
#include "PeakValidatorFactory.h"
#include "PFactorPeakValidator.h"
#include "SelectedPeakValidator.h"

namespace nsx {

PeakValidatorFactory::PeakValidatorFactory()
{
    registerCallback("masked" ,&MaskedPeakValidator::create);
    registerCallback("selected",&IndexedPeakValidator::create);
    registerCallback("indexed",&SelectedPeakValidator::create);
    registerCallback("pvalue",&PFactorPeakValidator::create);
}

} // end namespace nsx
