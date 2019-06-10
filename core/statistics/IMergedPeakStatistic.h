//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/IMergedPeakStatistic.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H
#define CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H

namespace nsx {

class MergedData;

//! \class IMergedPeakStatistic
//! Interface for statistics of merged peaks (e.g. R factors, correlation
//! coefficient, chi-squared).
class IMergedPeakStatistic {
public:
    virtual ~IMergedPeakStatistic() = default;
    //! Calculate the statistic on a given merged data set.
    virtual void calculate(const MergedData& data) = 0;
    //! Retrieve the value of the computed statistic.
    double value() const;
};

} // namespace nsx

#endif // CORE_STATISTICS_IMERGEDPEAKSTATISTIC_H
