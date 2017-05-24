
// 2d convolution via FFT
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#ifndef NSXLIB_CONVOLVER_H
#define NSXLIB_CONVOLVER_H

#include <complex>
#include <vector>

#include <fftw3.h>

#include <Eigen/Core>

#include "../mathematics/MathematicsTypes.h"

namespace nsx {

class Convolver
{
public:
    Convolver();

    // kernel must be padded so that dimensions match dimensions of image
    Convolver(const RealMatrix& kernel);

    ~Convolver();

    void reset();
    void setKernel(const RealMatrix& kernel);

    // throws exception if image dimensions do not match kernel
    RealMatrix apply(const RealMatrix& image);

private:
    int _rows, _cols, _halfCols;

    // used directly with FFTW3
    fftw_plan _forwardPlan;
    fftw_plan _backwardPlan;

    double* _realData;
    fftw_complex* _transformedData;
    std::vector<std::complex<double>> _transformedKernel;
};

} // end namespace nsx

#endif // NSXLIB_CONVOLVER_H
