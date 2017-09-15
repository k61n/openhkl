#include "AggregateFrameInterval.h"

namespace nsx {

AggregateFrameInterval::~AggregateFrameInterval()
{
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

AggregateFrameInterval::iterator AggregateFrameInterval::begin()
{
    return _intervals.begin();
}

AggregateFrameInterval::const_iterator AggregateFrameInterval::cbegin() const
{
    return _intervals.cbegin();
}

AggregateFrameInterval::iterator AggregateFrameInterval::end()
{
    return _intervals.end();
}

AggregateFrameInterval::const_iterator AggregateFrameInterval::cend() const
{
    return _intervals.cend();
}

} // end namespace nsx

