//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/RFactor.h
//! @brief     Defines class RFactor
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_RFACTOR_H
#define OHKL_CORE_STATISTICS_RFACTOR_H

#include "core/statistics/IMergedPeakStatistic.h" // inherits from
#include "core/statistics/MergedPeakCollection.h"

namespace ohkl {

//! IMergedPeakStatistic to compute various R factors.

class RFactor : public IMergedPeakStatistic {
 public:
    RFactor(bool sum_intensities)
        : _Rmerge(0.0)
        , _Rmeas(0.0)
        , _Rpim(0.0)
        , _expectedRmerge(0.0)
        , _expectedRmeas(0.0)
        , _expectedRpim(0.0)
        , _sum_intensities(sum_intensities)
    {
    }
    ~RFactor() { }

    //! Compute all R-factors
    void calculate(MergedPeakCollection* data) override;

    double Rmerge() const { return _Rmerge; }
    double Rmeas() const { return _Rmeas; }
    double Rpim() const { return _Rpim; }

    //! Expected value of Rmerge, based on error estimates
    double expectedRmerge() const { return _expectedRmerge; }
    //! Expected Rmeas, based on error estimates
    double expectedRmeas() const { return _expectedRmeas; }
    //! Expected Rpim, based on error estimates
    double expectedRpim() const { return _expectedRpim; }

 private:
    double _Rmerge, _Rmeas, _Rpim;
    double _expectedRmerge, _expectedRmeas, _expectedRpim;
    bool _sum_intensities; // use pixel sum intensities if true, otherwise profile
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_RFACTOR_H
