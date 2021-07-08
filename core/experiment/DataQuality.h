//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/DataQuality.h
//! @brief     Handler for computing data quality metrix in Experiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_DATAQUALITY_H
#define NSX_CORE_EXPERIMENT_DATAQUALITY_H

#include <string>
#include <vector>

namespace nsx {

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

    void computeQuality(MergedData& merged_peaks);
    std::string toString() const;
    void log() const;
};

struct MergeParameters {
    double d_min = 1.5; //!< Minimum d (Bragg's law)
    double d_max = 50.0; //!< Maximum d (Bragg's law)
    int n_shells = 10; //!< Number of resolution shells
};

struct ShellQuality : DataQuality {
    double dmin; //!< Lower limit of d for resolution shell
    double dmax; //!< Upper limit of d for resolution shell

    void computeQuality(MergedData& merged_peaks, double d_min, double d_max);
    void setCompleteness(const double c);
    std::string toString() const;
};

struct DataResolution {
    std::vector<ShellQuality> shells;

    void computeQuality(
        double d_min, double d_max, int n_shells, std::vector<PeakCollection*> collections, 
        SpaceGroup spacegroup, bool friedel);
    void log() const;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_DATAQUALITY_H
