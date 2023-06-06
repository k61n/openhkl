//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/CC.h
//! @brief     Defines class CC
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_CC_H
#define OHKL_CORE_STATISTICS_CC_H

#include "core/statistics/MergedPeak.h"

namespace ohkl {

class MergedData;

//! Calculates correlation coefficients (CChalf and CC*).

class CC {
 public:
    CC(bool sum_intensity);
    //! Calculate the statistic on the given set of merged peaks.
    void calculate(std::vector<MergedPeak> peaks);
    //! Calculate the statistic on the given data.
    void calculate(MergedData* data);
    //! Returns CC half (correlation of a random split of data into two bags)
    double CChalf() const;
    //! Returns CC star (estimate)
    double CCstar() const;
    //! Returns number of peaks
    unsigned int nPeaks() const;

 private:
    double _CChalf;
    double _CCstar;
    bool _sum_intensity;
    unsigned int _nPeaks{0};
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_CC_H
