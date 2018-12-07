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
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "GeometryTypes.h"
#include "Units.h"

namespace nsx {

//! \class FFTIndexing
//! \brief Class used for Fast-Fourier transform autoindexing.
class FFTIndexing {
public:
    //! Type to store vector together with quality.
    using tVector = std::pair<Eigen::RowVector3d, double>;
    //! Constructor. Note that the success of FFT indexing is very sensitive to choosing reasonable
    //! values of nSubdiva and amax.
    FFTIndexing(int nSubdiv=25,double amax=50.0);
    //! Method to search uniformly on the unit sphere to identify candidate lattice directions.
    //! It is crucial to choose reasonable values of n_vertices and nsolutions.
    std::vector<tVector> findOnSphere(const std::vector<ReciprocalVector>& qvects, unsigned int n_vertices, unsigned int nsolutions) const;
    virtual ~FFTIndexing() = default;

private:
    int _nSubdiv;
    double _amax;
};

} // end namespace nsx
