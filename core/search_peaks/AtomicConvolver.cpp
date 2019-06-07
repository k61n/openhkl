//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/search_peaks/AtomicConvolver.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cstring>
#include <stdexcept>
#include <utility>

#include "AtomicConvolver.h"

namespace nsx {

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

void AtomicConvolver::updateKernel(int nrows, int ncols)
{
    reset();

    _n_rows = nrows;
    _n_cols = ncols;

    RealMatrix kernel = _matrix(nrows, ncols);

    // Used by FFTW; check documentation for details
    _halfCols = (ncols >> 1) + 1;

    // Use fftw_malloc instead of fftw_alloc_* to support older version of fftw3
    _realData = (double*)fftw_malloc(nrows * ncols * sizeof(double));
    _transformedData = (fftw_complex*)fftw_malloc(nrows * _halfCols * sizeof(fftw_complex));

    _transformedKernel.resize(nrows * _halfCols);

    // Create plans
    _forwardPlan = fftw_plan_dft_r2c_2d(nrows, ncols, _realData, _transformedData, FFTW_MEASURE);
    _backwardPlan = fftw_plan_dft_c2r_2d(nrows, ncols, _transformedData, _realData, FFTW_MEASURE);

    // Precompute the transformation of the kernel
    std::memcpy(_realData, kernel.data(), nrows * ncols * sizeof(double));
    fftw_execute(_forwardPlan);

    // Store transformed kernel as vector of complexes (convenient for
    // convolution)
    for (int i = 0; i < nrows * _halfCols; ++i) {
        _transformedKernel[i] =
            std::complex<double>(_transformedData[i][0], _transformedData[i][1]);
    }
}

RealMatrix AtomicConvolver::convolve(const RealMatrix& image)
{
    int nrows = image.rows();
    int ncols = image.cols();

    if (nrows != _n_rows || ncols != _n_cols) {
        updateKernel(nrows, ncols);
    }

    // factor needed to get correct inverse transform
    double factor = 1.0 / ((double)(nrows * ncols));

    // precompute the transformation of the kernel
    std::memcpy(_realData, image.data(), nrows * ncols * sizeof(double));
    fftw_execute(_forwardPlan);

    // multiply fourier modes component-by-component
    for (int i = 0; i < nrows * _halfCols; ++i) {
        auto result = factor * _transformedKernel[i]
            * std::complex<double>(_transformedData[i][0], _transformedData[i][1]);
        _transformedData[i][0] = result.real();
        _transformedData[i][1] = result.imag();
    }

    // Perform inverse transform: _realData now stores the convolution
    fftw_execute(_backwardPlan);

    RealMatrix result(nrows, ncols);
    memcpy(result.data(), _realData, nrows * ncols * sizeof(double));
    return result;
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
