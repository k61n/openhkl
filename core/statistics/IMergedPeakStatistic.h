#pragma once

#include "CrystalTypes.h"

namespace nsx {

class MergedData;

//! \class IMergedPeakStatistic
//! \brief Interface for statistics of merged peaks (e.g. R factors, correlation
//! coefficient, chi-squared).
class IMergedPeakStatistic {
public:
    virtual ~IMergedPeakStatistic() = default;
    //! Calculate the statistic on a given merged data set.
    virtual void calculate(const MergedData& data) = 0;
    //! Retrieve the value of the computed statistic.
    double value() const;
};

} // end namespace nsx
