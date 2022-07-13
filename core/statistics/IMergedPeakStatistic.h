//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/IMergedPeakStatistic.h
//! @brief     Defines class IMergedPeakStatistic
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H
#define NSX_CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H

namespace ohkl {

class MergedData;

//! Pure virtual base class for statistics of merged peaks. Only realised by RFactor.

class IMergedPeakStatistic {
 public:
    virtual ~IMergedPeakStatistic() = default;
    //! Calculate the statistic on a given merged data set.
    virtual void calculate(MergedData* data) = 0;
    //! Retrieve the value of the computed statistic.
    double value() const;
};

} // namespace ohkl

#endif // NSX_CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H
