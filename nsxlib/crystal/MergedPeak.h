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

 Forshungszentrum Juelich GmbH
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

#ifndef NSXTOOL_MERGEDPEAK_H_
#define NSXTOOL_MERGEDPEAK_H_

#include <map>
#include <memory>

#include <Eigen/Dense>

#include "UnitCell.h"
#include "SpaceGroup.h"
#include "Peak3D.h"

namespace SX
{

namespace Crystal
{

class MergedPeak
{
public:
    MergedPeak(SpaceGroup grp, bool friedel=false);
    MergedPeak(const MergedPeak& other);
    ~MergedPeak();

    bool addPeak(sptrPeak3D peak);
    Eigen::Vector3i getIndex() const;
    double intensity() const;
    double sigma() const;
    double chiSquared() const;
    int redundancy() const;
    double std() const;

private:
    void determineRepresentativeHKL();
    void update();

    Eigen::Vector3i _hkl;
    double _intensity, _sigma, _chiSquared, _std;
    std::vector<sptrPeak3D> _peaks;
    SX::Crystal::SpaceGroup _grp;
    bool _friedel;
};



} // namespace Crystal

} // namespace SX

#endif /* NSXTOOL_SIMPLEPEAK_H_ */
