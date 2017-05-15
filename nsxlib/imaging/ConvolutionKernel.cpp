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
#include "AnnularKernel.h"

namespace nsx {

using RealMatrix = nsx::Types::RealMatrix;

ConvolutionKernel::ConvolutionKernel(int nrows, int ncols) : _kernel(), _hasChanged(true), _params()
{
    _kernel.resize(nrows,ncols);
}

ConvolutionKernel::ConvolutionKernel(const ConvolutionKernel &rhs)
{
    _kernel = rhs._kernel;
    _hasChanged = rhs._hasChanged;
    _params = rhs._params;
}

ConvolutionKernel::ConvolutionKernel(int nrows, int ncols, const ConvolutionKernel::ParameterMap &parameters)
{
    _kernel.resize(nrows,ncols);
    _params = parameters;
    _hasChanged = true;
}

ConvolutionKernel::~ConvolutionKernel()
{
}

ConvolutionKernel::ParameterMap &ConvolutionKernel::getParameters()
{
    _hasChanged = true;
    return _params;
}

const ConvolutionKernel::ParameterMap &ConvolutionKernel::getParameters() const
{
    return _params;
}

const RealMatrix& ConvolutionKernel::getKernel()
{
    if ( _hasChanged ) {
        update();
        _hasChanged = false;
    }
    return _kernel;
}

void ConvolutionKernel::print(std::ostream& os) const
{
    os << "Kernel Matrix (" << _kernel.rows() << "," << _kernel.cols() << "):" << std::endl;
    os << _kernel << std::endl;
}

ConvolutionKernel &ConvolutionKernel::operator=(const ConvolutionKernel &rhs)
{
    _kernel = rhs._kernel;
    _hasChanged = rhs._hasChanged;
    _params = rhs._params;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const ConvolutionKernel& kernel)
{
    kernel.print(os);
    return os;
}

} // end namespace nsx

