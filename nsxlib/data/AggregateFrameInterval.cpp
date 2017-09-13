/*
 * AggregateFrameInterval.cpp
 *
 *  Created on: Sep 13, 2017
 *      Author: pellegrini
 */

#include <nsxlib/data/AggregateFrameInterval.h>

namespace nsx {

AggregateFrameInterval::~AggregateFrameInterval() {
    for (auto ptr : _intervals) {
        delete ptr;
    }
    _intervals.clear();
}

PeakSet AggregateFrameInterval::peaks() const
{
    PeakSet selected_peaks;

    for (auto interval : _intervals) {
        auto&& peaks = interval->peaks();
        selected_peaks.insert(peaks.begin(),peaks.end());
    }
    return selected_peaks;
}

void AggregateFrameInterval::addInterval(IFrameInterval* interval)
{
    _intervals.insert(interval);
}

} /* namespace nsx */
