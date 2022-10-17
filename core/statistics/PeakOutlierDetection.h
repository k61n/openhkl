//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakOutlierDetection.h
//! @brief     Defines class PeakOutlierDetection
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_PEAKOUTLIERDETECTION_H
#define OHKL_CORE_STATISTICS_PEAKOUTLIERDETECTION_H

#include <vector>

namespace ohkl {

class PeakCollection;
class Peak3D;

/*! \addtogroup python_api
 *  @{*/


/*! \brief Reject outliers from a collection
 *
 * Class to manage outlier rejection
 */
class PeakOutlierDetection {
 public:
    PeakOutlierDetection(PeakCollection* peaks);

    void setPeakCollection(PeakCollection* peaks);
    void computeOutliers(unsigned int k, bool normalise);
    void getOutliers(double threshold, std::vector<std::pair<double, Peak3D*>>& outliers);

 private:
    PeakCollection* _peaks;
    std::vector<Peak3D*> _unmerged_peaks;
    std::vector<std::pair<double, Peak3D*>> _outliers;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_PEAKOUTLIERDETECTION_H
