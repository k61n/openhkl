#pragma once

#include "../crystal/CrystalTypes.h"
#include "../data/DataTypes.h"


namespace nsx {

class FrameInterval {

public:
    FrameInterval(sptrDataSet data);
    FrameInterval(sptrDataSet data, double fmin, double fmax);
    ~FrameInterval()=default;

    virtual PeakSet peaks() const;

private:
    const double _fmin;
    const double _fmax;
    sptrDataSet _data;
};

} // end namespace nsx
