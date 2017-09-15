#pragma once

#include "../crystal/CrystalTypes.h"
#include "../data/DataTypes.h"

namespace nsx {

class IFrameInterval {

public:

    IFrameInterval(sptrDataSet data);

    virtual ~IFrameInterval()=0;

    virtual PeakSet peaks() const=0;

protected:

    sptrDataSet _data;

};

} // end namespace nsx
