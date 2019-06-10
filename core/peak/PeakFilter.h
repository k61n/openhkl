//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/PeakFilter.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_PEAKFILTER_H
#define CORE_PEAK_PEAKFILTER_H

namespace nsx {

//! \class PeakFilter
//! Class to remove "bad" peaks based on various critera.
class PeakFilter {
public:
    //! Filter peaks that are complementary to the given peaks
    PeakList complementary(const PeakList& peaks, const PeakList& other_peaks) const;

    //! Filter only enabled peaks
    PeakList enabled(const PeakList& peaks, bool flag) const;

    //! Filter only selected peaks
    PeakList selected(const PeakList& peaks, bool flag) const;

    //! Filter only masked peaks
    PeakList masked(const PeakList& peaks, bool flag) const;

    //! Filter only peaks indexed by their unit cell and its indexing tolerance
    PeakList indexed(const PeakList& peaks) const;

    //! Filter only peaks indexed by a given unit cell with a given indexing
    //! tolerance
    PeakList indexed(const PeakList& peaks, const UnitCell& cell, double tolerance) const;

    //! Keeps only the peaks whose unit cell is the given unit cell
    PeakList unitCell(const PeakList& peaks, sptrUnitCell unit_cell) const;

    //! Remove peaks whitout unit cell
    PeakList hasUnitCell(const PeakList& peaks) const;

    //! Filter peaks with I/sigma above threshold
    PeakList strength(const PeakList& peaks, double min, double max) const;

    //! Filter those peaks which are predicted
    PeakList predicted(const PeakList& peaks, bool flag) const;

    //! Remove peaks which are not in a d-range
    PeakList dRange(const PeakList& peaks, double dmin, double max) const;

    //! Filter merged peaks which satisfies a chi2 test
    PeakList mergedPeaksSignificance(const PeakList& peaks, double significance_level) const;

    //! Remove overlapping peaks
    PeakList overlapping(const PeakList& peaks);

    //! Remove space-group extincted peaks
    PeakList extincted(const PeakList& peaks);

    //! Remove peaks which belongs to datasets containing too few peaks
    PeakList sparseDataSet(const PeakList& peaks, size_t min_num_peaks);
};

} // namespace nsx

#endif // CORE_PEAK_PEAKFILTER_H
