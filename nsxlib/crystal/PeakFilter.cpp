#include <algorithm>

#include "PeakFilter.h"
#include "PeakValidator.h"

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
    PeakSet validated_peaks;

    for (auto peak : peaks) {
        bool is_valid = true;
        for (auto validator : _validators) {
            if (!validator->isValid(peak)) {
                is_valid = false;
                break;
            }
        }
        if (is_valid) {
            validated_peaks.insert(peak);
        }
    }

    return validated_peaks;
}

void PeakFilter::addValidator(PeakValidator* validator) {

    auto it=std::find(_validators.begin(),_validators.end(),validator);
    if (it == _validators.end()) {
        _validators.push_back(validator);
    }
}

} // end namespace nsx
