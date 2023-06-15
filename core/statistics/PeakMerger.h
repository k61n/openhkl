//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakMerger.h
//! @brief     Defines class PeakMerger
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_PEAKMERGER_H
#define OHKL_CORE_STATISTICS_PEAKMERGER_H

#include "base/utils/Logger.h"
#include "base/utils/ProgressHandler.h"
#include "core/experiment/DataQuality.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/ResolutionShell.h"

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

//! Parameters for merging peaks
struct MergeParameters {
    double d_min = 1.5; //!< Minimum d (Bragg's law)
    double d_max = 50.0; //!< Maximum d (Bragg's law)
    int frame_min = -1; //!< First image to include
    int frame_max = -1; //!< Last image to include
    size_t n_shells = 10; //!< Number of resolution shells
    bool friedel = true; //!< Whether to include the Friedel relation
    double scale = 1.0; //!< Scale factor for intensities (Phenix only)

    void log(const Level& level) const;
};


/*! \brief Merge symmetry-related peaks and compute statistics
 *
 * Class to manage peak merging, i.e. merging peaks that are symmetry-equivalent.
 * Can also secondarily be used to merge distinct peak collections.
 * Computes statistical measures R-factor, CC to sanity check the quality of the
 * refinement and integration.
 */
class PeakMerger {
 public:
    PeakMerger(PeakCollection* peaks = nullptr);

    //! Add a peak collection to be merged
    void addPeakCollection(PeakCollection* peaks);
    //! Set the space group
    void setSpaceGroup(const SpaceGroup& group);
    //! Reset peak collections and merged data
    void reset();

    //! Perform the merge
    void mergePeaks();
    //! Perform the merge on predictions for strategy tool
    std::vector<double> strategyMerge(double fmin, double fmax, std::size_t nslices);
    //! Compute R-factors, CCs and completeness
    void computeQuality();

    //! Return a pointer to the parameter structure
    MergeParameters* parameters() const;

    //! Get a pointer to the sum integrated MergedPeakCollection object
    MergedPeakCollection* sumMergedPeakCollection() const;
    //! Get a pointer to the sum integrated merged data per resolution shell
    std::vector<MergedPeakCollection*> sumMergedPeakCollectionPerShell() const;
    //! Get a pointer to the profile integrated MergedPeakCollection object
    MergedPeakCollection* profileMergedPeakCollection() const;
    //! Get a pointer to the profile integrated merged data per resolution shell
    std::vector<MergedPeakCollection*> profileMergedPeakCollectionPerShell() const;

    //! Get the quality sum integrated statistics per resolution shell
    DataResolution* sumShellQuality();
    //! Get the overall sum integrated quality statistics
    DataResolution* sumOverallQuality();
    //! Get the quality profile integrated statistics per resolution shell
    DataResolution* profileShellQuality();
    //! Get the overall profile integrated quality statistics
    DataResolution* profileOverallQuality();
    //! Get the maximum possible completeness
    double maxCompleteness() const { return _max_completeness; }; 

    //! Return a string containing a summary of statistics
    std::string summary();

    //! Saves the shell information to file.
    bool saveStatistics(std::string filename);

    //! Set the progress handler
    void setHandler(sptrProgressHandler handler) { _handler = handler; };

 private:
    std::unique_ptr<MergedPeakCollection> _sum_merged_data;
    std::vector<std::unique_ptr<MergedPeakCollection>> _sum_merged_data_per_shell;
    std::unique_ptr<MergedPeakCollection> _profile_merged_data;
    std::vector<std::unique_ptr<MergedPeakCollection>> _profile_merged_data_per_shell;
    std::vector<PeakCollection*> _peak_collections;

    DataResolution _sum_shell_qualities;
    DataResolution _sum_overall_quality;
    DataResolution _profile_shell_qualities;
    DataResolution _profile_overall_quality;
    double _max_completeness;

    std::unique_ptr<MergeParameters> _params;

    SpaceGroup _space_group;

    sptrProgressHandler _handler;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_PEAKMERGER_H
