#pragma once

#include "../crystal/CrystalTypes.h"
#include "../data/DataTypes.h"
#include "../data/IFrameInterval.h"

namespace nsx {

class SimpleFrameInterval : public IFrameInterval {

public:

    SimpleFrameInterval(DataSet& data);

    SimpleFrameInterval(DataSet& data, int index_min, int index_max);

    ~SimpleFrameInterval()=default;

    virtual PeakSet peaks() const override;

private:

    int _index_min;
    int _index_max;
};

} // end namespace nsx
