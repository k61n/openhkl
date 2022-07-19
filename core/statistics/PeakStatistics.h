//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakStatistics.h
//! @brief     Defines class PeakStatistics
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_PEAKSTATISTICS_H
#define OHKL_CORE_STATISTICS_PEAKSTATISTICS_H

namespace ohkl {

class PeakCollection;

/*! \addtogroup python_api
 *  @{*/


/*! \brief Compute peaks statistics to facilitate outlier rejection
 *
 * Class to manage peak statistics.
 */
class PeakStatistics {
 public:
    PeakStatistics(PeakCollection* peaks);


 private:
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_PEAKSTATISTICS_H
