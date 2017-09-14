#pragma once

#include <set>

#include "IFrameInterval.h"

namespace nsx {

class AggregateFrameInterval : public IFrameInterval {

public:

    using IFrameInterval::IFrameInterval;

    virtual ~AggregateFrameInterval();

    virtual PeakSet peaks() const override;

    void addInterval(IFrameInterval* interval);

private:

    std::set<IFrameInterval*> _intervals;

};

} /* namespace nsx */
