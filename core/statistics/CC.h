#pragma once

#include "MergedPeak.h"

namespace nsx {
class MergedData;

//! Class to handle calculation of correlation coefficients (CChalf and CC*)
class CC {
public:
    //! Default constructor
    CC();
    //! Calculate the statistic on the given set of merged peaks.
    void calculate(const std::vector<MergedPeak>& peaks);
    //! Calculate the statistic on the given data.
    void calculate(const MergedData& data);
    //! Return CC half (correlation of a random split of data into two bags)
    double CChalf() const;
    //! Return CC star (estimate)
    double CCstar() const;
    //! Return number of peaks
    unsigned int nPeaks() const;

private:
    double _CChalf;
    double _CCstar;
    unsigned int _nPeaks;
};

} // end namespace nsx
