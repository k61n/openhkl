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
#include <mutex>
#include <stdexcept>
#include <utility>

#include "AtomicConvolver.h"

namespace nsx {

std::mutex g_mutex;

AtomicConvolver::AtomicConvolver()
    : Convolver()
    , _n_rows(0)
    , _n_cols(0)
    , _halfCols(0)
    , _forwardPlan(nullptr)
    , _backwardPlan(nullptr)
    , _realData(nullptr)
    , _transformedData(nullptr)
    , _transformedKernel()
{
}

AtomicConvolver::AtomicConvolver(const std::map<std::string, double>& parameters)
    : Convolver(parameters)
    , _n_rows(0)
    , _n_cols(0)
    , _halfCols(0)
    , _forwardPlan(nullptr)
    , _backwardPlan(nullptr)
    , _realData(nullptr)
    , _transformedData(nullptr)
    , _transformedKernel()
{
}

AtomicConvolver::~AtomicConvolver()
{
    reset();
}

void AtomicConvolver::reset()
{
    // FFTW is not thread safe for all its function but fftw_execute
    // See http://www.fftw.org/fftw3_doc/Thread-safety.html
    std::unique_lock<std::mutex> lock(g_mutex);

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

    _n_rows = 0;

    _n_cols = 0;

    _halfCols = 0;

    _forwardPlan = _backwardPlan = nullptr;

    _realData = nullptr;

    _transformedData = nullptr;

    _transformedKernel.resize(0);
}

void AtomicConvolver::updateKernel(int n_rows, int n_cols)
{
    if (n_rows == _n_rows && n_cols == _n_cols) {
        return;
    }

    reset();

    // FFTW is not thread safe for all its function but fftw_execute
    // See http://www.fftw.org/fftw3_doc/Thread-safety.html
    std::unique_lock<std::mutex> lock(g_mutex);

    _n_rows = n_rows;
    _n_cols = n_cols;

    const auto kernel_size = kernelSize();
    const int kernel_n_rows = kernel_size.first;
    const int kernel_n_cols = kernel_size.second;
    const int n_enlarged_rows = _n_rows + 2 * kernel_n_rows;
    const int n_enlarged_cols = _n_cols + 2 * kernel_n_cols;

    RealMatrix kernel = _matrix(n_enlarged_rows, n_enlarged_cols);
    const int n_pixels = kernel.size();

    // Used by FFTW; check documentation for details
    _halfCols = (n_enlarged_cols >> 1) + 1;

    // Use fftw_malloc instead of fftw_alloc_* to support older version of fftw3
    _realData = (double*)fftw_malloc(n_pixels * sizeof(double));
    _transformedData =
        (fftw_complex*)fftw_malloc(n_enlarged_rows * _halfCols * sizeof(fftw_complex));

    _transformedKernel.resize(n_enlarged_rows * _halfCols);

    // Create plans
    _forwardPlan = fftw_plan_dft_r2c_2d(
        n_enlarged_rows, n_enlarged_cols, _realData, _transformedData, FFTW_MEASURE);
    _backwardPlan = fftw_plan_dft_c2r_2d(
        n_enlarged_rows, n_enlarged_cols, _transformedData, _realData, FFTW_MEASURE);

    // Precompute the transformation of the kernel
    std::memcpy(_realData, kernel.data(), n_pixels * sizeof(double));
    fftw_execute(_forwardPlan);

    // Store transformed kernel as vector of complexes (convenient for convolution)
    for (int i = 0; i < n_enlarged_rows * _halfCols; ++i) {
        _transformedKernel[i] =
            std::complex<double>(_transformedData[i][0], _transformedData[i][1]);
    }
}

RealMatrix AtomicConvolver::convolve(const RealMatrix& image)
{
    const int n_rows = image.rows();
    const int n_cols = image.cols();

    updateKernel(n_rows, n_cols);

    const auto kernel_size = kernelSize();
    const int kernel_n_rows = kernel_size.first;
    const int kernel_n_cols = kernel_size.second;
    const int n_enlarged_rows = _n_rows + 2 * kernel_n_rows;
    const int n_enlarged_cols = _n_cols + 2 * kernel_n_cols;

    // Create an enlarged and double-casted version of the initial matrix
    RealMatrix padded_image(n_enlarged_rows, n_enlarged_cols);

    padded_image.block(kernel_n_rows, kernel_n_cols, n_rows, n_cols) = image;

    // Fill the enlarged matrix by mirroring the original image
    for (int r = 0; r < n_enlarged_rows; ++r) {
        int ii;
        if (r < kernel_n_rows) {
            ii = kernel_n_rows - r - 1;
        } else if (r >= n_rows + kernel_n_rows) {
            ii = 2 * n_rows + kernel_n_rows - r - 1;
        } else {
            ii = r - kernel_n_rows;
        }
        for (int c = 0; c < n_enlarged_cols; c++) {
            int jj;
            if (c < kernel_n_cols) {
                jj = kernel_n_cols - c - 1;
            } else if (c >= n_cols + kernel_n_cols) {
                jj = 2 * n_cols + kernel_n_cols - c - 1;
            } else {
                jj = c - kernel_n_cols;
            }

            if (r < kernel_n_rows || r >= n_enlarged_rows - kernel_n_rows || c < kernel_n_cols
                || c >= n_enlarged_cols - kernel_n_cols) {
                padded_image(r, c) = image(ii, jj);
            }
        }
    }

    const int n_pixels = padded_image.size();

    // factor needed to get correct inverse transform
    const double factor = 1.0 / ((double)(n_pixels));

    // precompute the transformation of the kernel
    std::memcpy(_realData, padded_image.data(), n_pixels * sizeof(double));
    fftw_execute(_forwardPlan);

    // multiply fourier modes component-by-component
    for (int i = 0; i < n_enlarged_rows * _halfCols; ++i) {
        auto result = factor * _transformedKernel[i]
            * std::complex<double>(_transformedData[i][0], _transformedData[i][1]);
        _transformedData[i][0] = result.real();
        _transformedData[i][1] = result.imag();
    }

    // Perform inverse transform: _realData now stores the convolution
    fftw_execute(_backwardPlan);

    RealMatrix result(n_enlarged_rows, n_enlarged_cols);
    memcpy(result.data(), _realData, n_pixels * sizeof(double));
    return result.block(kernel_n_rows, kernel_n_cols, n_rows, n_cols);
}

RealMatrix AtomicConvolver::matrix(int nrows, int ncols) const
{
    // sanity checks
    if (nrows < 0 || ncols < 0) {
        throw std::runtime_error("Invalid dimensions for kernel matrix");
    }

    return _matrix(nrows, ncols);
}

} // end namespace nsx
