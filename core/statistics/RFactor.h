#pragma once

#include <set>
#include <vector>

#include "CrystalTypes.h"
#include "IMergedPeakStatistic.h"

namespace nsx {

class MergedData;

//! \brief Class used to compute the various R factors.
class RFactor : public IMergedPeakStatistic {
public:
    RFactor()
        : _Rmerge(0.0)
        , _Rmeas(0.0)
        , _Rpim(0.0)
        , _expectedRmerge(0.0)
        , _expectedRmeas(0.0)
        , _expectedRpim(0.0)
    {
    }
    ~RFactor() {}

    void calculate(const MergedData& data);

    double Rmerge() { return _Rmerge; }
    double Rmeas() { return _Rmeas; }
    double Rpim() { return _Rpim; }

    //! Expected value of Rmerge, based on error estimates
    double expectedRmerge() { return _expectedRmerge; }
    //! Expected Rmeas, based on error estimates
    double expectedRmeas() { return _expectedRmeas; }
    //! Expected Rpim, based on error estimates
    double expectedRpim() { return _expectedRpim; }

private:
    double _Rmerge, _Rmeas, _Rpim;
    double _expectedRmerge, _expectedRmeas, _expectedRpim;
};

} // end namespace nsx
