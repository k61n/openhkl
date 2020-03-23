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

#ifndef CORE_ANALYSE_PEAKFILTER_H
#define CORE_ANALYSE_PEAKFILTER_H

#include "core/shape/PeakCollection.h"
#include <bitset>

namespace nsx {

//! Class to remove "bad" peaks based on various critera.

struct FilterParameters {
    bool selected;      //! filter by selection
    bool masked;        //! filter by mask
    bool predicted;     //! filter by predicted peaks
    bool indexed;       //! filter by indexed peaks
    bool index_tol;     //!
    bool state;         //!
    bool strength;      //! filter by strength (intensity/sigma)
    bool d_range;       //! filter by detector range
    bool extinct;       //! filter by extinction (allowed by unit cell)
    bool sparse;        //!
    bool significance;  //!
    bool overlapping;   //!
    bool complementary; //!
};

class PeakFilter {

 public:
    //! Constructor
    PeakFilter();

 public:
    //! Set filter parameters
    void setFilterSelected(bool val) { _filter_params.selected = val; };
    void setFilterMasked(bool val) { _filter_params.masked = val; };
    void setFilterPredicted(bool val) { _filter_params.predicted = val; };
    void setFilterIndexed(bool val) { _filter_params.indexed = val; };
    void setFilterIndexTol(bool val) { _filter_params.index_tol = val; };
    void setFilterState(bool val) { _filter_params.state = val; };
    void setFilterStrength(bool val) { _filter_params.strength = val; };
    void setFilterDRange(bool val) { _filter_params.d_range = val; };
    void setFilterExtinct(bool val) { _filter_params.extinct = val; };
    void setFilterSparse(bool val) { _filter_params.sparse = val; };
    void setFilterSignificance(bool val) { _filter_params.significance = val; };
    void setFilterOverlapping(bool val) { _filter_params.overlapping = val; };
    void setFilterComplementary(bool val) { _filter_params.complementary = val; };

    bool getFilterSelected() { return _filter_params.selected; };
    bool getFilterMasked() { return _filter_params.masked; };
    bool getFilterPredicted() { return _filter_params.predicted; };
    bool getFilterIndexed() { return _filter_params.indexed; };
    bool getFilterIndexTol() { return _filter_params.index_tol; };
    bool getFilterState() { return _filter_params.state; };
    bool getFilterStrength() { return _filter_params.strength; };
    bool getFilterDRange() { return _filter_params.d_range; };
    bool getFilterExtinct() { return _filter_params.extinct; };
    bool getFilterSparse() { return _filter_params.sparse; };
    bool getFilterSignificance() { return _filter_params.significance; };
    bool getFilterOverlapping() { return _filter_params.overlapping; };
    bool getFilterComplementary() { return _filter_params.complementary; };

    //! set filter parameters to default
    void resetFilterParameters();

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
    void filterExtincted(PeakCollection* peak_collection) const;

    //! Remove peaks which belongs to datasets containing too few peaks
    void filterSparseDataSet(PeakCollection* peak_collection) const;

 public:
    //! Filter only enabled on a peak vector
    std::vector<Peak3D*> filterEnabled(const std::vector<Peak3D*> peaks_ptr, bool flag) const;

    //! Filter only enabled on a peak vector
    std::vector<Peak3D*> filterIndexed(
        const std::vector<Peak3D*> peaks_ptr, const UnitCell& cell, double tolerance) const;

 public:
    //! Run the filtering
    void filter(PeakCollection* peak_collection) const;

    //! Reset filtering
    void resetFiltering(PeakCollection* peak_collection) const;

    //! Return the d range values
    const std::array<double, 2>* dRange() const { return &_d_range; };
    //! Set the d range values
    void setDRange(const std::array<double, 2> d_range) { _d_range = d_range; };

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

    //! Return the significance
    const double* significance() const { return &_significance; };
    //! Set the significance
    void setSignificance(const double significance) { _significance = significance; };

 private:
    //! booleans for filtering
    FilterParameters _filter_params;
    //! The values for range
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
};

} // namespace nsx

#endif // CORE_ANALYSE_PEAKFILTER_H
