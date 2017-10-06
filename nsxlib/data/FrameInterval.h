#pragma once

#include "../crystal/CrystalTypes.h"
#include "../data/DataTypes.h"


namespace nsx {

class SimpleFrameInterval {

public:

    SimpleFrameInterval(sptrDataSet data);

    SimpleFrameInterval(sptrDataSet data, size_t index_min, size_t index_max);

    ~SimpleFrameInterval()=default;

    virtual PeakSet peaks() const;

private:

    const int _index_min;
    const int _index_max;
    sptrDataSet _data;
};

} // end namespace nsx
