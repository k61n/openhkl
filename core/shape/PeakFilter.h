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
    bool frames; //! catch peaks in a specifed frame range
};

class PeakFilter {
 public:
    //! Constructor
    PeakFilter();

 public:
    //! Set filter parameters
    void setFilterSelected(bool val) { _filter_flags.selected = val; };
    void setFilterMasked(bool val) { _filter_flags.masked = val; };
    void setFilterPredicted(bool val) { _filter_flags.predicted = val; };
    void setFilterIndexed(bool val) { _filter_flags.indexed = val; };
    void setFilterIndexTol(bool val) { _filter_flags.index_tol = val; };
    void setFilterState(bool val) { _filter_flags.state = val; };
    void setFilterStrength(bool val) { _filter_flags.strength = val; };
    void setFilterDRange(bool val) { _filter_flags.d_range = val; };
    void setFilterExtinct(bool val) { _filter_flags.extinct = val; };
    void setFilterSparse(bool val) { _filter_flags.sparse = val; };
    void setFilterSignificance(bool val) { _filter_flags.significance = val; };
    void setFilterOverlapping(bool val) { _filter_flags.overlapping = val; };
    void setFilterComplementary(bool val) { _filter_flags.complementary = val; };
    void setFilterFrames(bool val) { _filter_flags.frames = val; };

    bool getFilterSelected() const { return _filter_flags.selected; };
    bool getFilterMasked() const { return _filter_flags.masked; };
    bool getFilterPredicted() const { return _filter_flags.predicted; };
    bool getFilterIndexed() const { return _filter_flags.indexed; };
    bool getFilterIndexTol() const { return _filter_flags.index_tol; };
    bool getFilterState() const { return _filter_flags.state; };
    bool getFilterStrength() const { return _filter_flags.strength; };
    bool getFilterDRange() const { return _filter_flags.d_range; };
    bool getFilterExtinct() const { return _filter_flags.extinct; };
    bool getFilterSparse() const { return _filter_flags.sparse; };
    bool getFilterSignificance() const { return _filter_flags.significance; };
    bool getFilterOverlapping() const { return _filter_flags.overlapping; };
    bool getFilterComplementary() const { return _filter_flags.complementary; };
    bool getFilterFrames() const { return _filter_flags.frames; };

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
        const std::vector<Peak3D*> peaks, const UnitCell& cell, double tolerance) const;

    //! Run the filtering
    void filter(PeakCollection* peak_collection) const;

    //! Reset filtering
    void resetFiltering(PeakCollection* peak_collection) const;

    //! Return the d range values
    const std::array<double, 2>* dRange() const { return &_d_range; };
    //! Set the d range values
    void setDRange(const std::array<double, 2> d_range) { _d_range = d_range; };
    void setDRange(double dmin, double dmax) { _d_range = {dmin, dmax}; };

    //! Return the unit cell name
    const std::string* unitCellName() const { return &_unit_cell; };
    //! Set the unit cell name
    void setUnitCellName(const std::string unit_cell) { _unit_cell = unit_cell; };

    //! Return the unit cell tolerance
    const double* unitCellTolerance() const { return &_unit_cell_tolerance; };
    //! Set the unit cell tolerance
    void setUnitCellTolerance(const double tolerance) { _unit_cell_tolerance = tolerance; };

    //! Return the strength
    const std::array<double, 2>* strength() const { return &_strength; };
    //! Set the strength
    void setStrength(const std::array<double, 2> strength) { _strength = strength; };
    void setStrength(double strmin, double strmax) { _strength = {strmin, strmax}; };

    //! Return the significance
    const double* significance() const { return &_significance; };
    //! Set the significance
    void setSignificance(const double significance) { _significance = significance; };

    //! Set the frame range
    void setFrameRange(const double f0, const double f1) { _frameRange = {f0, f1}; };
    void setFrameRange(const std::array<double, 2> frames) { _frameRange = frames; };
    //! Return the frame range
    const std::array<double, 2>& frameRange() { return _frameRange; };

 private:
    //! booleans for filtering
    PeakFilterFlags _filter_flags;
    //! Detector range
    std::array<double, 2> _d_range;
    //! The unit cell name
    std::string _unit_cell;
    //! The unit cell tolerance
    double _unit_cell_tolerance;
    //! The strength
    std::array<double, 2> _strength;
    //! The significance
    double _significance;
    //! number of peaks in dataset to be kept
    double _sparse;
    //! Frame range
    std::array<double, 2> _frameRange;
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_PEAKFILTER_H
