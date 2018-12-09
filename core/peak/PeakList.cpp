#include "PeakList.h"
#include "Peak3D.h"

namespace nsx {

void maskPeaks(const DataSet *const dataSet, PeakList& peaks)
{
    for (auto peak: peaks) {
        if (peak->data().get() != dataSet)
            continue; // peak belongs to another dataset
        peak->setMasked(false);
        for (const auto& m : dataSet->masks()) {
            // If the background of the peak intercept the mask, unselected the peak
            if (m->collide(peak->shape())) {
                peak->setMasked(true);
                break;
            }
        }
    }
}

} // namespace nsx
