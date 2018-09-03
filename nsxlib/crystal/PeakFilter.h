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
    //! Apply the filter to the given data set.
    PeakList apply(const PeakList& peaks) const;
    //! Filter peaks that are complementary to the given peaks
    PeakList complementary(const PeakList& peaks, const PeakList& other_peaks) const;
    //! Filter only selected peaks
    PeakList enabled(const PeakList& peaks, bool selection_flag) const;
    //! Filter only peaks indexed by the given cell
    PeakList indexed(const PeakList& peaks, double tolerance) const;
    //! Filter only peaks indexed by the given cell
    PeakList indexed(const PeakList& peaks, sptrUnitCell cell, double tolerance) const;
    //! Filter peaks associated to the given dataset
    PeakList dataset(const PeakList& peaks, sptrDataSet dataset) const;
    //! Filter peaks associated with the given unit cell
    PeakList unitCell(const PeakList& peaks, sptrUnitCell unit_cell) const;
    //! Filter peaks with sigma above threshold
    PeakList minSigma(const PeakList& peaks, double threshold) const;
    //! Filter peaks with I/sigma above threshold
    PeakList signalToNoise(const PeakList& peaks, double threshold) const;
    //! Filter peaks with I below threshold
    PeakList lowIntensity(const PeakList& peaks, double threshold) const;
    //! Filter those peaks which are predicted
    PeakList predicted(const PeakList& peaks) const;
    //! Remove peaks which are not in a d-range
    PeakList dRange(const PeakList& peaks, double dmin, double max) const;
    //! Filter peaks by minimum d
    PeakList dMin(const PeakList& peaks, double dmin) const;
    //! Filter peaks by maximum d
    PeakList dMax(const PeakList& peaks, double dmax) const;
    //! Return peaks with given indices
    PeakList selection(const PeakList& peaks, const std::vector<int>& indexes) const;
    //! Filter peaks which have a unit cell
    PeakList hasUnitCell(const PeakList& peaks) const;
    //! Filter peaks by raw p value
    PeakList significance(const PeakList& peaks, double p_value_threshold) const;

public:
    //! Remove peaks which are not indexed (incommensurate).
    bool _removeUnindexed;
    //! Remove peaks which are not selected.
    bool _removeUnselected;
    //! Remove peaks based on cutoff for \f$I/\sigma_I\f$.
    bool _removeIsigma;
    //! Remove peak based on cutoff for raw p-value.
    bool _removePValue;
    //! Remove peaks if they are overlapping.
    bool _removeOverlapping;
    //! Remove peaks with \f$d < d_{\mathrm{min}}\f$.
    bool _removeDmin;
    //! Remove peaks with \f$d > d_{\mathrm{max}}\f$.
    bool _removeDmax;
    //! Remove reflections which are forbidden by spacegroup symmetry.
    bool _removeForbidden;
    //! Remove peaks based on cutoff for merged p-value (chi-squared test).
    bool _removeMergedP;
    //! Value of the \f$I/\sigma_I\f$ cutoff.
    double _Isigma;
    //! Value of the minimum d cutoff.
    double _dmin;
    //! Value of the maximum d cutoff.
    double _dmax;
    //! Value of the raw p-value cutoff.
    double _pvalue;
    //! Value of the merged p-value cutoff.
    double _mergedP;
};

} // end namespace nsx
