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

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "Convolver.h"

namespace nsx {

Convolver::Convolver()
: _kernel(),
  _halfCols(0),
  _forwardPlan(nullptr),
  _backwardPlan(nullptr),
  _realData(nullptr),
  _transformedData(nullptr),
  _transformedKernel()
{
}

Convolver::~Convolver()
{
    reset();
}

void Convolver::reset()
{
    if (_forwardPlan) {
        fftw_destroy_plan(_forwardPlan);
    }
    if (_backwardPlan) {
        fftw_destroy_plan(_backwardPlan);
    }

    if (_realData) {
        fftw_free(_realData);
    }
    if (_transformedData) {
        fftw_free(_transformedData);
    }

    _halfCols = 0;

    _forwardPlan = _backwardPlan = nullptr;

    _realData = nullptr;

    _transformedData = nullptr;

    _transformedKernel.resize(0);
}

void Convolver::updateKernel(int nrows, int ncols)
{
    reset();

    _kernel = _matrix(nrows,ncols);

    // Used by FFTW; check documentation for details
    _halfCols = (ncols>>1) + 1;

    // Use fftw_malloc instead of fftw_alloc_* to support older version of fftw3
    _realData = (double*)fftw_malloc(nrows * ncols * sizeof(double));
    _transformedData = (fftw_complex*)fftw_malloc(nrows * _halfCols * sizeof(fftw_complex));

    _transformedKernel.resize(nrows*_halfCols);

    // Create plans
    _forwardPlan = fftw_plan_dft_r2c_2d(nrows, ncols, _realData, _transformedData, FFTW_MEASURE);
    _backwardPlan = fftw_plan_dft_c2r_2d(nrows, ncols, _transformedData, _realData, FFTW_MEASURE);

    // Precompute the transformation of the kernel
    std::memcpy(_realData, _kernel.data(), nrows*ncols*sizeof(double));
    fftw_execute(_forwardPlan);

    // store transformed kernel as vector of complexes (convenient for convolution)
    for (int i = 0; i < nrows*_halfCols; ++i) {
        _transformedKernel[i] = std::complex<double>(_transformedData[i][0], _transformedData[i][1]);
    }
}

RealMatrix Convolver::convolve(const RealMatrix& image)
{
    int nrows = image.rows();
    int ncols = image.cols();

    updateKernel(nrows,ncols);

    // factor needed to get correct inverse transform
    double factor = 1.0 / ((double)(nrows*ncols));

    // precompute the transformation of the kernel
    std::memcpy(_realData, image.data(), nrows*ncols*sizeof(double));
    fftw_execute(_forwardPlan);

    // multiply fourier modes component-by-component
    for (int i = 0; i < nrows*_halfCols; ++i) {
        auto result = factor * _transformedKernel[i] * std::complex<double>(_transformedData[i][0], _transformedData[i][1]);
        _transformedData[i][0] = result.real();
        _transformedData[i][1] = result.imag();
    }

    // Perform inverse transform: _realData now stores the convolution
    fftw_execute(_backwardPlan);

    RealMatrix result(nrows, ncols);
    memcpy(result.data(), _realData, nrows*ncols*sizeof(double));
    return result;
}

std::map<std::string,double>& Convolver::parameters()
{
    return _parameters;
}

const std::map<std::string,double>& Convolver::parameters() const
{
    return _parameters;
}

void Convolver::setParameters(const std::map<std::string,double>& parameters)
{
    for (auto p : parameters) {
        auto it = _parameters.find(p.first);
        if (it != _parameters.end()) {
            it->second = p.second;
        }
    }
}

RealMatrix Convolver::matrix(int nrows, int ncols) const
{
    // sanity checks
    if (nrows < 0 || ncols < 0) {
        throw std::runtime_error("Invalid dimensions for kernel matrix");
    }

    return _matrix(nrows,ncols);
}

} // end namespace nsx

