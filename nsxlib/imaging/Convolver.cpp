
// 2d convolution via FFT
// author: Jonathan Fisher
// Forshungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include "Convolver.h"
#include <fftw3.h>
#include <cstring>

#include <iostream>

using std::cout;
using std::endl;


namespace SX
{

namespace Imaging
{

using RealMatrix = SX::Types::RealMatrix;


Convolver::Convolver():
    _rows(0), _cols(0), _halfCols(0),
    _forwardPlan(nullptr), _backwardPlan(nullptr),
    _realData(nullptr), _transformedData(nullptr), _transformedKernel()
{

}

Convolver::Convolver(const RealMatrix &kernel): Convolver()
{
    setKernel(kernel);
}

Convolver::~Convolver()
{
    reset();
}

void Convolver::reset()
{
    if (_forwardPlan) fftw_destroy_plan(_forwardPlan);
    if (_backwardPlan) fftw_destroy_plan(_backwardPlan);

    if (_realData) fftw_free(_realData);
    if (_transformedData) fftw_free(_transformedData);

    _rows = _cols = _halfCols = 0;
    _forwardPlan = _backwardPlan = nullptr;
    _realData = nullptr;
    _transformedData = nullptr;

    _transformedKernel.resize(0);
}

void Convolver::setKernel(const RealMatrix &kernel)
{
    // check whether we need a new plan
    if ( kernel.rows() != _rows || kernel.cols() != _cols) {
        reset();

        _rows = kernel.rows();
        _cols = kernel.cols();

        _halfCols = (_cols>>1) + 1; // used by FFTW; check documentation for details

        // use fftw_malloc instead of fftw_alloc_* to support older version of fftw3
        _realData = (double*)fftw_malloc(_rows * _cols * sizeof(double));
        _transformedData = (fftw_complex*)fftw_malloc(_rows * _halfCols * sizeof(fftw_complex));
        
        _transformedKernel.resize(_rows*_halfCols);

        cout << "allocated fftw arrays successfully" << endl;

        // create plans
        _forwardPlan = fftw_plan_dft_r2c_2d(_rows, _cols, _realData, _transformedData, FFTW_MEASURE);
        _backwardPlan = fftw_plan_dft_c2r_2d(_rows, _cols, _transformedData, _realData, FFTW_MEASURE);

        cout << "created fftw plans successfully" << endl;
    }

    // precompute the transformation of the kernel
    std::memcpy(_realData, kernel.data(), _rows*_cols*sizeof(double));
    fftw_execute(_forwardPlan);

    cout << "forward transform of kernel computed successfully" << endl;

    // store transformed kernel as vector of comlexes (convenient for convolution)
    for (int i = 0; i < _rows*_halfCols; ++i)
        _transformedKernel[i] = std::complex<double>(_transformedData[i][0], _transformedData[i][1]);

    cout << "convolver initialized kernel successfully" << endl;
}

RealMatrix Convolver::apply(const RealMatrix &image)
{
    // image MUST have same dimensions as kernel
    if ( _rows != image.rows() || _cols != image.cols() )
        throw std::runtime_error("Convolver::apply() received image with incorrect dimensions");

    // factor needed to get correct inverse transform
    double factor = 1.0 / ((double)(_rows*_cols));

    // precompute the transformation of the kernel
    std::memcpy(_realData, image.data(), _rows*_cols*sizeof(double));
    fftw_execute(_forwardPlan);

    cout << "forward plan executed" << endl;

    // multiply fourier modes component-by-component
    for (int i = 0; i < _rows*_halfCols; ++i) {
        auto result = factor * _transformedKernel[i]
                * std::complex<double>(_transformedData[i][0], _transformedData[i][1]);
        _transformedData[i][0] = result.real();
        _transformedData[i][1] = result.imag();
    }

    cout << "multiplication complete successully" << endl;

    // perform inverse transform: _realData now stores the convolution
    fftw_execute(_backwardPlan);

    cout <<"backward plan executed successfully";

    RealMatrix result(_rows, _cols);
    memcpy(result.data(), _realData, _rows*_cols*sizeof(double));
    return result;
}

} // Imaging
} // SX
