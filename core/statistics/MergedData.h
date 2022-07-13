//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/MergedData.h
//! @brief     Defines class MergedData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_STATISTICS_MERGEDDATA_H
#define NSX_CORE_STATISTICS_MERGEDDATA_H

#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeak.h"

#include <set>

namespace ohkl {

//! A set of `MergedPeak`s.

using MergedPeakSet = std::set<MergedPeak>;

//! Contains a set of `MergedPeak`s and crystallographic information (SpaceGroup and Friedel flag).

class MergedData {
 public:
    //! Construct merged dataset with given spacegroup. Paramter \p friedel
    //! determines whether to include the Friedel relation \f$q \mapsto -q\f$, if
    //! this is not already part of the space group symmetry.
    MergedData(
        SpaceGroup space_group, std::vector<PeakCollection*> peak_collections, bool friedel,
        int fmin = -1, int fmax = -1);
    MergedData(SpaceGroup space_group, bool friedel, int fmin = -1, int fmax = -1);
    //! Add a peak to the data set. It will be automatically merged correctly
    //! according to the space group symmetry.
    bool addPeak(Peak3D* peak);
    //! Return the set of merged peaks.
    const MergedPeakSet& mergedPeakSet() const;
    //! Returns redundancy = observations / symmetry-inequvialent peaks.
    double redundancy() const;
    //! Returns total number of observations (including redundant ones)
    size_t totalSize() const;
    //! Return theoretical maximum number of peaks
    int maxPeaks() const { return _max_peaks; };
    //! Clear the merged data
    void clear();
    //! Set the d range
    void setDRange(double d_min, double d_max);
    //! Get minimum d
    double dMin() const;
    //! Get maximum d
    double dMax() const;

    SpaceGroup spaceGroup() const { return _group; };

 private:
    SpaceGroup _group;
    bool _friedel;
    MergedPeakSet _merged_peak_set;
    std::vector<PeakCollection*> _peak_collections;
    double _d_min;
    double _d_max;
    int _frame_min;
    int _frame_max;

    //! Number of invalid peaks
    int _nInvalid = 0;
    int _nPeaks = 0;
    int _nNoCell = 0;
    int _nDisabled = 0;
    int _nBadInterp = 0;
    int _nDupes = 0;
    int _nExtinct = 0;
    int _max_peaks = 0;
};

} // namespace ohkl

#endif // NSX_CORE_STATISTICS_MERGEDDATA_H
