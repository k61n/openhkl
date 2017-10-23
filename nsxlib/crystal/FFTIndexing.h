/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

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

#include <functional>
#include <vector>

#include <Eigen/Dense>

#include "MathematicsTypes.h"
#include "Units.h"

namespace nsx {

struct tVector {
    tVector(const Eigen::Vector3d& v,double quality):_vect(v),_quality(quality) {}
    Eigen::Vector3d _vect;
    double _quality;
};

class FFTIndexing {
public:
    FFTIndexing(int nSubdiv=25,double amax=50.0);
    std::vector<tVector> findOnSphere(const std::vector<Eigen::Vector3d>& qvects, unsigned int nstacks, unsigned int nsolutions) const;
    virtual ~FFTIndexing() = default;

private:
    int _nSubdiv;
    double _amax;
};

} // end namespace nsx
