//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/CC.h
//! @brief     Defines class CC
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_STATISTICS_CC_H
#define CORE_STATISTICS_CC_H

#include "core/analyse/MergedData.h"
#include "core/analyse/MergedPeak.h"

namespace nsx {

//! Calculates correlation coefficients (CChalf and CC*).

class CC {
 public:
    CC();
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
    unsigned int _nPeaks;
};

} // namespace nsx

#endif // CORE_STATISTICS_CC_H
