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
    unsigned int frame_min = 1;
    unsigned int frame_max = 0;
    size_t n_shells = 10; //!< Number of resolution shells
    bool friedel = true; //!< Whether to include the Friedel relation

    void log(const Level& level) const;
};

//! Class to manage peak merging, i.e. merging peaks that are symmetry-equivalent.
//! Can also secondarily be used to merge distinct peak collections.
//! Computes statistical measures R-factor, CC to sanity check the quality of the
//! refinement and integration.
class PeakMerger {
 public:
    PeakMerger(PeakCollection* peaks = nullptr);

    //! Add a peak collection to be merged
    void addPeakCollection(PeakCollection* peaks);
    //! Reset peak collections and merged data
    void reset();
    //! Set the default frame range
    void setFrameRange(unsigned int min, unsigned int max);

    //! Perform the merge
    void mergePeaks();
    //! Compute R-factors, CCs and completeness
    void computeQuality();

    //! Return a pointer to the parameter structure
    MergeParameters* parameters() const;

    //! Get a pointer to the MergedData object
    MergedData* getMergedData() const;
    //! Get a pointer to the merged data per resolution shell
    std::vector<MergedData*> getMergedDataPerShell() const;

    //! Get the quality statistics per resolution shell
    const DataResolution* shellQuality();
    //! Get the overall quality statistics
    const DataResolution* overallQuality();

 private:
    std::unique_ptr<MergedData> _merged_data;
    std::vector<std::unique_ptr<MergedData>> _merged_data_per_shell;
    std::vector<PeakCollection*> _peak_collections;

    DataResolution _shell_qualities;
    DataResolution _overall_quality;

    std::unique_ptr<MergeParameters> _params;
};

} // namespace nsx

#endif // NSX_CORE_STATISTICS_PEAKMERGER_H
