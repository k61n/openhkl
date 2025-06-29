//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/IMergedPeakStatistic.h
//! @brief     Defines class IMergedPeakStatistic
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H
#define OHKL_CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H

namespace ohkl {

class MergedPeakCollection;

//! Pure virtual base class for statistics of merged peaks. Only realised by RFactor.

class IMergedPeakStatistic {
 public:
    virtual ~IMergedPeakStatistic() = default;
    //! Calculate the statistic on a given merged data set.
    virtual void calculate(MergedPeakCollection* data) = 0;
    //! Retrieve the value of the computed statistic.
    double value() const;
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H
