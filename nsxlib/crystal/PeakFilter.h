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

    void filter(sptrDataSet data) const;
    bool invalid(sptrDataSet data, sptrPeak3D peak) const;

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

