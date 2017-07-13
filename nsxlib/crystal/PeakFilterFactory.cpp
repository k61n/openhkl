#include "MaskedPeakFilter.h"
#include "IndexedPeakFilter.h"
#include "PeakFilterFactory.h"
#include "PValuePeakFilter.h"
#include "SelectedPeakFilter.h"

namespace nsx {

PeakFilterFactory::PeakFilterFactory()
{
    registerCallback("masked" ,&MaskedPeakFilter::create);
    registerCallback("selected",&SelectedPeakFilter::create);
    registerCallback("indexed",&IndexedPeakFilter::create);
    registerCallback("pvalue",&PValuePeakFilter::create);
}

} // end namespace nsx
