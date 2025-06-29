//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/MergedPeak.h
//! @brief     Defines class MergedPeak
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_MERGEDPEAK_H
#define OHKL_CORE_STATISTICS_MERGEDPEAK_H

#include "core/peak/Intensity.h"
#include "tables/crystal/SpaceGroup.h"

namespace ohkl {

class Peak3D;

enum class MergeFlag { Added, Inequivalent, Invalid };

//! A list of peaks, and crystallographic information (SpaceGroup, MillerIndex, ...).
class MergedPeak {
 public:
    //! Construct a merged peak with the given spacegroup.
    //!
    //! The resulting peak has intensity given by the average intensity of the
    //! input peaks. The parameter \p friedel specifies whether to also include
    //! the Friedel relation \f$(h,k,l)\mapsto(-h,-k,-l)\f$, if this is not
    //! already part of the space group symmetry.
    MergedPeak(const SpaceGroup& grp, bool sum_intensity, bool friedel = false);

    //! Add a peak to the merged peak.
    MergeFlag addPeak(Peak3D* peak);
    //! Add a peak regardless of RejectionFlag, do not sum intensity
    MergeFlag addAny(Peak3D* peak);

    //! Returns a representative Miller index of the peak.
    MillerIndex index() const;

    //! Returns the merged intensity of the peak.
    Intensity intensity() const;

    //! Returns the redundancy (number of symmetry-equivalent observations) of the merged peak.
    size_t redundancy() const;

    //! Compute the chi-squared statistic of a merged peak.
    double chi2() const;

    //! Compute the p-value of the chi-squared statistic of the merged peak.
    double pValue() const;

    //! Returns vector of peaks used to compute the merged peak.
    std::vector<Peak3D*> peaks() const;

    //! split the merged peak randomly into two, for calculation of CC
    std::pair<MergedPeak, MergedPeak> split(bool sum_intensity) const;

    //! Return list of symmetry-related peaks as a string
    std::string toString() const;

 private:
    //! Update the hkl that represents the set of equivalences.
    void determineRepresentativeHKL();
    void update();

    MillerIndex _hkl;
    Intensity _intensity;
    std::vector<Peak3D*> _peaks;
    SpaceGroup _grp;
    bool _sum_intensity;
    bool _friedel;
};

#ifndef SWIG
bool operator<(const MergedPeak& p, const MergedPeak& q);
#endif

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_MERGEDPEAK_H
