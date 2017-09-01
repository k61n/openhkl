#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "../data/DataTypes.h"

namespace nsx {

class PeakFilter {

public:
    PeakFilter();
    ~PeakFilter();

    //! Apply the filter to the given data set. Returns the number of removed peaks
    int apply(sptrDataSet data) const;

public:
    bool _removeUnindexed;
    bool _removeMasked;
    bool _removeUnselected;
    bool _removeMultiplyIndexed;
    bool _removeIsigma;
    bool _removeSignificance;
    bool _removeOverlapping;
    bool _removeDmin;
    bool _removeDmax;

    double _Isigma;
    double _dmin;
    double _dmax;
    double _significance;


};

} // end namespace nsx

