
// 2d convolution via FFT
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_INCLUDE_CONVOLVER_H_
#define NSXTOOL_INCLUDE_CONVOLVER_H_

#include <Eigen/Core>
#include <vector>
#include <fftw3.h>
#include <vector>
#include <complex>

#include "../utils/Types.h"


namespace SX
{

namespace Imaging
{

class Convolver
{
public:
    Convolver();

    // kernel must be padded so that dimensions match dimensions of image
    Convolver(const SX::Types::RealMatrix& kernel);

    ~Convolver();

    void reset();
    void setKernel(const SX::Types::RealMatrix& kernel);

    // throws exception if image dimensions do not match kernel
    SX::Types::RealMatrix apply(const SX::Types::RealMatrix& image);

private:
    int _rows, _cols, _halfCols;

    // used directly with FFTW3
    fftw_plan _forwardPlan;
    fftw_plan _backwardPlan;

    double* _realData;
    fftw_complex* _transformedData;
    std::vector<std::complex<double>> _transformedKernel;
};

} // Imaging
} // SX

#endif // NSXTOOL_INCLUDE_CONVOLVER_H_
