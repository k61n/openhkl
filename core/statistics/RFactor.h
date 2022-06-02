//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/RFactor.h
//! @brief     Defines class RFactor
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_STATISTICS_RFACTOR_H
#define NSX_CORE_STATISTICS_RFACTOR_H

#include "core/statistics/IMergedPeakStatistic.h" // inherits from

namespace nsx {

//! IMergedPeakStatistic to compute various R factors.

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
    ~RFactor() { }

    void calculate(MergedData* data);

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

} // namespace nsx

#endif // NSX_CORE_STATISTICS_RFACTOR_H
