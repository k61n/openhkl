//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/DataQuality.h
//! @brief     Handler for computing data quality metrix in Experiment
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_DATAQUALITY_H
#define OHKL_CORE_EXPERIMENT_DATAQUALITY_H

#include "base/utils/Logger.h"

#include <string>
#include <vector>

namespace ohkl {

class MergedData;
class RFactor;
class CC;
class PeakCollection;
class ResolutionShell;
class SpaceGroup;

struct DataQuality {
    int nobserved; //!< Total number of observations including redundant
    int nunique; //!< Total number of observations excluding redundant
    double redundancy; //!< observations / symmetry unique peaks
    double Rmerge; //!< R-factor
    double expectedRmerge; //!< expected R-factor
    double Rmeas; //!< multiplicity-weighted R-factor
    double expectedRmeas; //!< expected multiplicity-weighted R-factor
    double Rpim; //!< relative (precision-indicating) R-factor
    double expectedRpim; //!< expected relative R-factor
    double CChalf; //!< CC_{1/2} correlation coefficient
    double CCstar; //!< estimate of CC_{true} derived from CC_{1/2}
    double Completeness; //!< # valid predicted peaks / # predicted peaks

    void computeQuality(MergedData& merged_peaks, bool sum_intensities = true);
    std::string toString() const;
    void log() const;
};

struct ShellQuality : DataQuality {
    double dmin; //!< Lower limit of d for resolution shell
    double dmax; //!< Upper limit of d for resolution shell

    void computeQuality(
        MergedData& merged_peaks, double d_min, double d_max, bool sum_intensities = true);
    std::string toString() const;
};

struct DataResolution {
    std::vector<ShellQuality> shells;

    void addShell(const ShellQuality& shell);
    void clear();
    std::string summary() const;
    void log() const;
};

} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_DATAQUALITY_H
