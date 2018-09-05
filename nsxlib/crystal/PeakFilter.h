#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "DataTypes.h"

namespace nsx {

//! \class PeakFilter
//! \brief Class to remove "bad" peaks based on various critera.
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
    //! Filter only peaks indexed by the given cell
    PeakList indexed(const PeakList& peaks) const;
    //! Filter only peaks indexed by the given cell
    PeakList indexed(const PeakList& peaks, sptrUnitCell cell, double tolerance) const;
    //! Filter peaks associated to the given dataset
    PeakList dataset(const PeakList& peaks, sptrDataSet dataset) const;
    //! Filter peaks associated with the given unit cell
    PeakList unitCell(const PeakList& peaks, sptrUnitCell unit_cell) const;
    //! Filter peaks with I/sigma above threshold
    PeakList strength(const PeakList& peaks, double min, double max) const;
    //! Filter those peaks which are predicted
    PeakList predicted(const PeakList& peaks) const;
    //! Remove peaks which are not in a d-range
    PeakList dRange(const PeakList& peaks, double dmin, double max) const;
    //! Return peaks with given indices
    PeakList selection(const PeakList& peaks, const std::vector<int>& indexes) const;
    //! Filter peaks which have a unit cell
    PeakList hasUnitCell(const PeakList& peaks) const;
    //! Filter merged peaks which satisfies a chi2 test
    PeakList mergedPeaksSignificance(const PeakList& peaks, double significance_level) const;
    //! Remove overlapping peaks
    PeakList overlapping(const PeakList& peaks);
    //! Remove space-group extincted peaks
    PeakList extincted(const PeakList& peaks);
    //! Remove peaks which belongs to datasets containing too few peaks
    PeakList sparseDataSet(const PeakList& peaks, size_t min_num_peaks);
};

} // end namespace nsx
