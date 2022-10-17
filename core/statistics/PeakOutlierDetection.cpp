//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakOutlierDetection.cpp
//! @brief     Implements class PeakOutlierDetection
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/PeakOutlierDetection.h"

#include "base/outlier/LocalOutlierFactor.h"
#include "base/utils/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"

namespace ohkl {

PeakOutlierDetection::PeakOutlierDetection(PeakCollection* peaks) : _peaks(peaks)
{
    setPeakCollection(peaks);
}

void PeakOutlierDetection::setPeakCollection(PeakCollection *peaks)
{
    _unmerged_peaks.clear();
    for (auto* peak : peaks->getPeakList()) {
        if (peak->enabled())
            _unmerged_peaks.emplace_back(peak);
    }
}

void PeakOutlierDetection::computeOutliers(unsigned int k, bool normalise)
{
    ohklLog(
        Level::Info,
        "PeakOutlierDetection::computeOutliers: Detecting outliers for peak collection ",
        _peaks->name());
    // Populate the data array
    int npoints = _unmerged_peaks.size();
    ohklLog(
        Level::Info, "PeakOutlierDetection::computeOutliers: ", npoints, " total peaks");
    Eigen::MatrixXd data(npoints, 2); // 2 dimensional data (intensity, sigma)
    for (std::size_t idx = 0; idx < npoints; ++idx) {
        data(idx, 0) = _unmerged_peaks.at(idx)->correctedIntensity().value();
        data(idx, 1) = _unmerged_peaks.at(idx)->correctedIntensity().sigma();
    }

    LocalOutlierFactor lof(k, 2, npoints, &data, normalise);

    _outliers.clear();
    for (const auto& [lof_value, index] : lof.findOutliers())
        _outliers.push_back({lof_value, _unmerged_peaks.at(index)});
    ohklLog(
        Level::Info, "PeakOutlierDetection::computeOutliers: ", _outliers.size(),
        " outliers found");
}

void PeakOutlierDetection::getOutliers(
    double threshold, std::vector<std::pair<double, Peak3D*>>& outliers)
{
    outliers.clear();
    for (const auto& [lof_value, peak] : _outliers) {
        if (lof_value > threshold) {
            outliers.push_back({lof_value, peak});
            ohklLog(
                Level::Debug, lof_value, " ", peak->hkl().h(), " ", peak->hkl().k(), " ",
                peak->hkl().l(), " ", peak->correctedIntensity().value(), " ",
                peak->correctedIntensity().sigma());
        }
    }
}

} // namespace ohkl
