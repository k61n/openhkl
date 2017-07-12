#include <algorithm>

#include "PeakFilter.h"

namespace nsx {

PeakFilter::PeakFilter() : _validators()
{
}

PeakFilter::~PeakFilter()
{
    for (auto v : _validators) {
        delete v;
    }
}

PeakSet PeakFilter::filter(const PeakSet& peaks) const
{
    PeakSet filteredPeaks;

    for (auto peak : peaks) {
        bool keepPeak = true;
        for (auto validator : _validators) {
            if (!validator->isValid(peak)) {
                keepPeak = false;
                break;
            }
        }
        if (keepPeak) {
            filteredPeaks.insert(peak);
        }
    }

    return filteredPeaks;
}

void PeakFilter::addValidator(PeakValidator* validator) {

    auto it=std::find(_validators.begin(),_validators.end(),validator);
    if (it == _validators.end()) {
        _validators.push_back(validator);
    }
}

} // end namespace nsx
