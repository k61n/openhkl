//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/merge_and_filter/MergedData.h
//! @brief     Defines class MergedData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_MERGEDDATA_H
#define CORE_PEAK_MERGEDDATA_H

#include "core/analyse/MergedPeak.h"

namespace nsx {

//! Class to handle merged datasets.
class MergedData {
public:
    //! Construct merged dataset with given spacegroup. Paramter \p friedel
    //! determines whether to include the Friedel relation \f$q \mapsto -q\f$, if
    //! this is not already part of the space group symmetry.
    MergedData(const SpaceGroup& grp, bool friedel);
    //! Add a peak to the data set. It will be automatically merged correctly
    //! according to the space group symmetry.
    bool addPeak(const sptrPeak3D& peak);
    //! Retern the set of merged peaks.
    const MergedPeakSet& peaks() const;

    //! Returns redundancy = observations / symmetry-inequvialent peaks.
    double redundancy() const;

    //! Returns total number of observations (including redundant ones)
    size_t totalSize() const;

    //! Clear the merged data
    void clear();

private:
    SpaceGroup _group;
    bool _friedel;
    MergedPeakSet _peaks;
};

} // namespace nsx

#endif // CORE_PEAK_MERGEDDATA_H
