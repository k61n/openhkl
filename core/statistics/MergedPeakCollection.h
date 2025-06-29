//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/MergedPeakCollection.h
//! @brief     Defines class MergedPeakCollection
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_MERGEDPEAKCOLLECTION_H
#define OHKL_CORE_STATISTICS_MERGEDPEAKCOLLECTION_H

#include "core/data/DataTypes.h"
#include "core/statistics/MergedPeak.h"

#include <set>

namespace ohkl {

class PeakCollection;

//! A set of `MergedPeak`s.

using MergedPeakSet = std::set<MergedPeak>;

//! Contains a set of `MergedPeak`s and crystallographic information (SpaceGroup and Friedel flag).

class MergedPeakCollection {
 public:
    //! Construct merged dataset with given spacegroup. Paramter \p friedel
    //! determines whether to include the Friedel relation \f$q \mapsto -q\f$, if
    //! this is not already part of the space group symmetry.
    MergedPeakCollection(
        SpaceGroup space_group, std::vector<PeakCollection*> peak_collections, bool friedel,
        bool sum_intensity, int fmin = -1, int fmax = -1);
    MergedPeakCollection(
        SpaceGroup space_group, bool friedel, bool sum_intensity, int fmin = -1, int fmax = -1);
    //! For computing maximum possible completeness
    MergedPeakCollection(
        SpaceGroup space_group, std::vector<PeakCollection*> peak_collections, bool friedel);
    //! Add a peak to the data set. It will be automatically merged correctly
    //! according to the space group symmetry.
    void addPeak(Peak3D* peak);
    //! Add a peak regardless of rejection status, do not track intensity
    void addAny(Peak3D* peak);
    //! Add a peak collection (without processing the peaks)
    void addPeakCollection(PeakCollection* peaks);
    //! Return the set of merged peaks.
    const MergedPeakSet& mergedPeakSet() const;
    //! Returns redundancy = observations / symmetry-inequvialent peaks.
    double redundancy() const;
    //! Returns total number of observations (including redundant ones)
    size_t totalSize() const;
    //! Returns number of symmetry-unique observation
    size_t nUnique() const { return _merged_peak_set.size(); };
    //! Return theoretical maximum number of peaks
    int maxPeaks() const { return _max_peaks; };
    //! Clear the merged data
    void clear();
    //! Set the d range
    void setDRange(double d_min, double d_max);
    //! Set the d range
    void setDRange(double d_min, double d_max, sptrDataSet data, sptrUnitCell cell);
    //! Get minimum d
    double dMin() const;
    //! Get maximum d
    double dMax() const;

    //! Output a list of unmerged peaks as a string, sorted by symmetry relations
    std::string toStringUnmerged() const;

    SpaceGroup spaceGroup() const { return _group; };

 private:
    SpaceGroup _group;
    bool _friedel;
    MergedPeakSet _merged_peak_set;
    std::vector<PeakCollection*> _peak_collections;
    bool _sum_intensity;
    double _d_min;
    double _d_max;
    int _first_frame;
    int _last_frame;

    //! Number of invalid peaks
    int _nInvalid = 0;
    int _nInequivalent = 0;
    int _max_peaks = 0;
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_MERGEDPEAKCOLLECTION_H
