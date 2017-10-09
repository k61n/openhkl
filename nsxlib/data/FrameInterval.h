#pragma once

#include "../crystal/CrystalTypes.h"
#include "../crystal/UBSolution.h"
#include "../data/DataTypes.h"

#include <map>

namespace nsx {

class FrameInterval {

public:
    FrameInterval(sptrDataSet data);
    FrameInterval(sptrDataSet data, double fmin, double fmax);
    ~FrameInterval()=default;
    PeakSet peaks() const;

private:
    const double _fmin;
    const double _fmax;
    sptrDataSet _data;
    std::map<UnitCell*, UBSolution> _soln;
};

} // end namespace nsx
