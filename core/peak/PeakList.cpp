#include "PeakList.h"
#include "Peak3D.h"
#include "PeakFilter.h"

namespace nsx {

void maskPeaks(const DataSet* const dataSet, PeakList& peaks)
{
    for (auto peak : peaks) {
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

// TODO: check whether this function is used at all
std::vector<PeakList>
findEquivalences(const SpaceGroup& spaceGroup, const PeakList& peaks, bool friedel)
{
    std::vector<PeakList> peak_equivs;

    for (auto peak : peaks) {
        bool found_equivalence = false;
        auto cell = peak->unitCell();

        PeakFilter peak_filter;
        PeakList same_cell_peaks = peak_filter.unitCell(peaks, cell);

        MillerIndex miller_index1(peak->q(), *cell);

        for (size_t i = 0; i < peak_equivs.size() && !found_equivalence; ++i) {
            MillerIndex miller_index2(peak_equivs[i][0]->q(), *cell);

            if ((friedel && spaceGroup.isFriedelEquivalent(miller_index1, miller_index2))
                || (!friedel && spaceGroup.isEquivalent(miller_index1, miller_index2))) {
                found_equivalence = true;
                peak_equivs[i].push_back(peak);
                continue;
            }
        }

        if (!found_equivalence)
            peak_equivs.emplace_back(PeakList({peak}));
    }
    return peak_equivs;
}

} // namespace nsx
