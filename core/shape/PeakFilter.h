//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/PeakFilter.h
//! @brief     Defines class PeakFilter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_SHAPE_PEAKFILTER_H
#define NSX_CORE_SHAPE_PEAKFILTER_H

#include "core/shape/PeakCollection.h"

namespace nsx {

//! Class to remove "bad" peaks based on various critera.

struct PeakFilterFlags {
    bool selected; //!< filter by selection
    bool masked; //!< filter by mask
    bool predicted; //!< filter by predicted peaks
    bool indexed; //!< filter by indexed peaks
    bool index_tol; //!
    bool state; //!
    bool strength; //!< filter by strength (intensity/sigma)
    bool d_range; //!< filter by detector range
    bool extinct; //!< filter by extinction (allowed by unit cell)
    bool sparse; //!
    bool significance; //!
    bool overlapping; //!
    bool complementary; //!
    bool frames; //!< catch peaks in a specifed frame range
};

struct PeakFilterParameters {
    double d_min = 1.5; //!< minimum d (Bragg's law)
    double d_max = 50.0; //!< maximum d (Bragg's law)
    double strength_min = 1.0; //!< minimum strength (I/sigma)
    double strength_max = 1.0e7; //!< maximum strength (I/sigma)
    std::string unit_cell = "DEFAULT UNIT CELL"; //!< unit cell name
    double unit_cell_tolerance = 0.2; //!< indexing tolerance
    double significance = 0.99; //!< signficance
    double sparse = 100; //!< number of peaks in dataset to be kept
    double frame_min = 0.0; //!< start of frame range
    double frame_max = 10.0; //!< end of frame range
    double peak_end = 3.0; //!< scale for peak intensity ellipsoid (sigmas)
    double bkg_end = 6.0; //!< scale for background ellipsoid (sigmas)
};

class PeakFilter {
 public:
    //! Constructor
    PeakFilter();

 public:
    //! set filter parameters to default
    void resetFilterFlags();

    //! Filter peaks that are complementary to the given peaks
    void filterComplementary(PeakCollection* peak_collection) const;

    //! Filter only enabled peaks
    void filterEnabled(PeakCollection* peak_collection) const;

    //! Filter only selected peaks
    void filterSelected(PeakCollection* peak_collection) const;

    //! Filter only masked peaks
    void filterMasked(PeakCollection* peak_collection) const;

    //! Filter only peaks indexed by their unit cell and its indexing tolerance
    void filterIndexed(PeakCollection* peak_collection) const;

    //! Filter only peaks indexed by a given unit cell with a given indexing
    //! tolerance
    void filterIndexTolerance(PeakCollection* peak_collection) const;

    //! Keeps only the peaks whose unit cell is the given unit cell
    void filterUnitCell(PeakCollection* peak_collection) const;

    //! Remove peaks whitout unit cell
    void filterHasUnitCell(PeakCollection* peak_collection) const;

    //! Filter peaks with I/sigma above threshold
    void filterStrength(PeakCollection* peak_collection) const;

    //! Filter those peaks which are predicted
    void filterPredicted(PeakCollection* peak_collection) const;

    //! Remove peaks which are not in a d-range
    void filterDRange(PeakCollection* peak_collection) const;

    //! Filter merged peaks which satisfies a chi2 test
    void filterSignificance(PeakCollection* peak_collection) const;

    //! Remove overlapping peaks
    void filterOverlapping(PeakCollection* peak_collection) const;

    //! Remove space-group extincted peaks
    void filterExtinct(PeakCollection* peak_collection) const;

    //! Remove peaks which belongs to datasets containing too few peaks
    void filterSparseDataSet(PeakCollection* peak_collection) const;

    //! Remove peaks outside the specified frame range
    void filterFrameRange(PeakCollection* peak_collection) const;

    //! Filter only enabled on a peak vector
    std::vector<Peak3D*> filterEnabled(const std::vector<Peak3D*> peaks, bool flag) const;

    //! Filter only enabled on a peak vector
    std::vector<Peak3D*> filterIndexed(
        const std::vector<Peak3D*> peaks, const UnitCell* cell = nullptr) const;

    //! Run the filtering
    void filter(PeakCollection* peak_collection) const;

    //! Reset filtering
    void resetFiltering(PeakCollection* peak_collection) const;

    //! Get a pointer to the filter parameters
    PeakFilterParameters* parameters();
    //! Get pointer to filter flags
    PeakFilterFlags* flags();

 private:
    //! booleans for filtering
    std::unique_ptr<PeakFilterFlags> _filter_flags;
    std::unique_ptr<PeakFilterParameters> _filter_params;
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_PEAKFILTER_H
