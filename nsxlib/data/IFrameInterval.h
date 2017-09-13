#pragma once

#include "../crystal/CrystalTypes.h"
#include "../data/DataTypes.h"

namespace nsx {

class IFrameInterval {

public:

    IFrameInterval(DataSet& data);

    virtual ~IFrameInterval()=0;

    virtual PeakSet peaks() const=0;

protected:

    DataSet& _data;

};

} // end namespace nsx
