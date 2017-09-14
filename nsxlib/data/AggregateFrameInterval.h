#pragma once

#include <set>

#include "IFrameInterval.h"

namespace nsx {

class AggregateFrameInterval : public IFrameInterval {

private:

    using FrameSet = std::set<IFrameInterval*>;

    FrameSet _intervals;

public:

    using IFrameInterval::IFrameInterval;

    virtual ~AggregateFrameInterval();

    virtual PeakSet peaks() const override;

    void addInterval(IFrameInterval* interval);

    using iterator = FrameSet::iterator;
    iterator begin();
    iterator end();

    using const_iterator = FrameSet::const_iterator;
    const_iterator cbegin() const;
    const_iterator cend() const;

};

} /* namespace nsx */
