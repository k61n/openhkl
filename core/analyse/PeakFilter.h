//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/analyse/PeakFilter.h
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

#include "core/peak/Peak3D.h" // PeakList
#include "core/peak/PeakCollection.h"
#include <bitset>

namespace nsx {

//! Class to remove "bad" peaks based on various critera.

class PeakFilter {

 public:
    //! Constructor
    PeakFilter();

 public:
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

    //! Return the booleans
    const std::bitset<13>* booleans() const { return &_filter_compute; };
    //! Set the booleans
    void setBooleans(const std::bitset<13> booleans) { _filter_compute = booleans; };

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
    //! The booleans for the filtering
    std::bitset<13> _filter_compute;
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
