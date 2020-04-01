//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef CORE_ANALYSE_MERGEDDATA_H
#define CORE_ANALYSE_MERGEDDATA_H

#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeak.h"

#include <set>

namespace nsx {

//! A set of `MergedPeak`s.

using MergedPeakSet = std::set<MergedPeak>;

//! Contains a set of `MergedPeak`s and crystallographic information (SpaceGroup and Friedel flag).

class MergedData {
 public:
    //! Construct merged dataset with given spacegroup. Paramter \p friedel
    //! determines whether to include the Friedel relation \f$q \mapsto -q\f$, if
    //! this is not already part of the space group symmetry.
    MergedData(std::vector<PeakCollection*> peak_collections, bool friedel);
    MergedData(SpaceGroup space_group, bool friedel);
    //! Add a peak to the data set. It will be automatically merged correctly
    //! according to the space group symmetry.
    bool addPeak(Peak3D* peak);
    //! Return the set of merged peaks.
    const MergedPeakSet& mergedPeakSet() const;
    //! Returns redundancy = observations / symmetry-inequvialent peaks.
    double redundancy() const;
    //! Returns total number of observations (including redundant ones)
    size_t totalSize() const;
    //! Clear the merged data
    void clear();

    //! Clear the merged data
    SpaceGroup spaceGroup() const { return _group; };

 private:
    SpaceGroup _group;
    bool _friedel;
    MergedPeakSet _merged_peak_set;
    std::vector<PeakCollection*> _peak_collections;
};

} // namespace nsx

#endif // CORE_ANALYSE_MERGEDDATA_H
