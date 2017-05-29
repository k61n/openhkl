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

#ifndef NSXLIB_MERGEDPEAK_H
#define NSXLIB_MERGEDPEAK_H

#include <Eigen/Dense>

#include "../crystal/CrystalTypes.h"
#include "../crystal/Intensity.h"
#include "../crystal/SpaceGroup.h"
#include "../crystal/PeakCalc.h"

namespace nsx {
    class Peak3D;
    using sptrPeak3D = std::shared_ptr<Peak3D>;

//! Class to handle calculation of merged data
class MergedPeak {

public:
    MergedPeak(const SpaceGroup& grp, bool friedel=false);
    MergedPeak(const MergedPeak& other) = default;
    ~MergedPeak() = default;

    bool addPeak(const sptrPeak3D& peak);
    bool addPeak(const PeakCalc& peak);

    Eigen::Vector3i getIndex() const;
    Intensity getIntensity() const;

    size_t redundancy() const;
    double std() const;

    const std::vector<PeakCalc>& getPeaks() const;

    //! split the merged peak randomly into two, for calculation of CC
    std::pair<MergedPeak, MergedPeak> split() const;

private:
    void determineRepresentativeHKL();
    void update();
    
    Eigen::Vector3i _hkl;
    Intensity _intensitySum;
    double _squaredIntensitySum;
    std::vector<PeakCalc> _peaks;
    SpaceGroup _grp;
    bool _friedel;
};

#ifndef SWIG
bool operator<(const MergedPeak& p, const MergedPeak& q);
#endif


} // end namespace nsx

#endif // NSXLIB_MERGEDPEAK_H
