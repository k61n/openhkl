/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "Intensity.h"
#include "PeakList.h"
#include "SpaceGroup.h"

namespace nsx {

//! Class to handle calculation of merged data
class MergedPeak {
public:

    //! \brief Construct a merged peak with the given spacegroup. 
    //!
    //! The resulting peak has intensity given by the average intensity of the input peaks.
    //! The parameter \p friedel
    //! specifies whether to also include the Friedel relation \f$(h,k,l)\mapsto(-h,-k,-l)\f$,
    //! if this is not already part of the space group symmetry.
    MergedPeak(const SpaceGroup& grp, bool friedel=false);

    //! Add a peak to the merged peak.
    bool addPeak(const sptrPeak3D& peak);

    //! Return a representative Miller index of the peak.
    Eigen::RowVector3i getIndex() const;

    //! Return the merged intensity of the peak.
    Intensity getIntensity() const;

    //! Return the redundancy (number of symmetry-equivalent observations) of the merged peak.
    size_t redundancy() const;

    //! Compute the chi-squared statistic of a merged peak.
    double chi2() const;

    //! Compute the p-value of the chi-squared statistic of the merged peak.
    double pValue() const;

    //! Return vector of peaks used to compute the merged peak.
    const PeakList& getPeaks() const;

    //! split the merged peak randomly into two, for calculation of CC
    std::pair<MergedPeak, MergedPeak> split() const;

private:

    //! Update the hkl that represents the set of equivalences.
    void determineRepresentativeHKL();
    void update();
    
    Eigen::RowVector3i _hkl;
    Intensity _intensitySum;
    PeakList _peaks;
    SpaceGroup _grp;
    bool _friedel;
};

#ifndef SWIG
bool operator<(const MergedPeak& p, const MergedPeak& q);
#endif

} // end namespace nsx
