//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/MergedPeak.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_MERGEDPEAK_H
#define CORE_PEAK_MERGEDPEAK_H

#include <Eigen/Dense>

#include "core/crystal/Intensity.h"
#include "core/crystal/MillerIndex.h"
#include "core/crystal/SpaceGroup.h"
#include "core/experiment/CrystalTypes.h"

namespace nsx {

//! \class MergedPeak
//! \brief Class to handle calculation of merged data
class MergedPeak {
public:
    //! \brief Construct a merged peak with the given spacegroup.
    //!
    //! The resulting peak has intensity given by the average intensity of the
    //! input peaks. The parameter \p friedel specifies whether to also include
    //! the Friedel relation \f$(h,k,l)\mapsto(-h,-k,-l)\f$, if this is not
    //! already part of the space group symmetry.
    MergedPeak(const SpaceGroup& grp, bool friedel = false);

    //! Add a peak to the merged peak.
    bool addPeak(const sptrPeak3D& peak);

    //! Returns a representative Miller index of the peak.
    MillerIndex index() const;

    //! Returns the merged intensity of the peak.
    Intensity intensity() const;

    //! Returns the redundancy (number of symmetry-equivalent observations) of the
    //! merged peak.
    size_t redundancy() const;

    //! Compute the chi-squared statistic of a merged peak.
    double chi2() const;

    //! Compute the p-value of the chi-squared statistic of the merged peak.
    double pValue() const;

    //! Returns vector of peaks used to compute the merged peak.
    const PeakList& peaks() const;

    //! split the merged peak randomly into two, for calculation of CC
    std::pair<MergedPeak, MergedPeak> split() const;

private:
    //! Update the hkl that represents the set of equivalences.
    void determineRepresentativeHKL();
    void update();

    MillerIndex _hkl;
    Intensity _intensitySum;
    PeakList _peaks;
    SpaceGroup _grp;
    bool _friedel;
};

#ifndef SWIG
bool operator<(const MergedPeak& p, const MergedPeak& q);
#endif

} // end namespace nsx

#endif // CORE_PEAK_MERGEDPEAK_H
