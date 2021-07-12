//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/PeakMerger.h
//! @brief     Defines class PeakMerger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_STATISTICS_PEAKMERGER_H
#define NSX_CORE_STATISTICS_PEAKMERGER_H

#include "base/utils/Logger.h"
#include "core/experiment/DataQuality.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/ResolutionShell.h"

namespace nsx {

struct MergeParameters {
    double d_min = 1.5; //!< Minimum d (Bragg's law)
    double d_max = 50.0; //!< Maximum d (Bragg's law)
    size_t n_shells = 10; //!< Number of resolution shells
    bool friedel = true; //!< Whether to include the Friedel relation

    void log(const Level& level) const;
};

class PeakMerger {
 public:
    PeakMerger(PeakCollection* peaks = nullptr);

    void addPeakCollection(PeakCollection* peaks);
    void reset();

    void mergePeaks();
    void computeQuality();

    MergeParameters* parameters() const ;
    void setParameters(std::shared_ptr<MergeParameters> params);

    MergedData* getMergedData() const;
    std::vector<MergedData*> getMergedDataPerShell() const;

    const DataResolution* shellQuality();
    const DataResolution* overallQuality();

 private:
    std::unique_ptr<MergedData> _merged_data;
    std::vector<std::unique_ptr<MergedData>> _merged_data_per_shell;
    std::vector<PeakCollection*> _peak_collections;

    DataResolution _shell_qualities;
    DataResolution _overall_quality;

    std::shared_ptr<MergeParameters> _params;
};

} // namespace nsx

#endif // NSX_CORE_STATISTICS_PEAKMERGER_H
