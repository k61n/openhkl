/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr
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

#include <iostream>
#include <stdexcept>
#include <utility>

#include "ConvolutionKernel.h"

namespace nsx {

ConvolutionKernel::~ConvolutionKernel()
{
}

std::map<std::string,double>& ConvolutionKernel::parameters()
{
    return _parameters;
}

const std::map<std::string,double>& ConvolutionKernel::parameters() const
{
    return _parameters;
}

RealMatrix ConvolutionKernel::matrix(int nrows, int ncols) const
{
    // sanity checks
    if (nrows < 0 || ncols < 0) {
        throw std::runtime_error("Invalid dimensions for kernel matrix");
    }

    return _matrix(nrows,ncols);
}

} // end namespace nsx

