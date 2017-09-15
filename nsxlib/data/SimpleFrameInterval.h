#pragma once

#include "../crystal/CrystalTypes.h"
#include "../data/DataTypes.h"
#include "../data/IFrameInterval.h"

namespace nsx {

class SimpleFrameInterval : public IFrameInterval {

public:

    SimpleFrameInterval(sptrDataSet data);

    SimpleFrameInterval(sptrDataSet data, size_t index_min, size_t index_max);

    ~SimpleFrameInterval()=default;

    virtual PeakSet peaks() const override;

private:

    const int _index_min;
    const int _index_max;
};

} // end namespace nsx
