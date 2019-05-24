#ifndef CORE_SEARCH_PEAKS_ATOMICCONVOLVER_H
#define CORE_SEARCH_PEAKS_ATOMICCONVOLVER_H

#include <complex>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fftw3.h>

#include <Eigen/Dense>

#include "Convolver.h"
#include "MathematicsTypes.h"

namespace nsx {

class AtomicConvolver : public Convolver {

public:
    AtomicConvolver();

    AtomicConvolver(const std::map<std::string, double>& parameters);

    AtomicConvolver(const AtomicConvolver& other) = default;

    AtomicConvolver& operator=(const AtomicConvolver& other) = default;

    virtual ~AtomicConvolver() = 0;

    Convolver* clone() const = 0;

    RealMatrix matrix(int nrows, int ncols) const;

    //! Convolve an image
    virtual RealMatrix convolve(const RealMatrix& image) override;

    virtual std::pair<size_t, size_t> kernelSize() const = 0;

protected:
    void updateKernel(int nrows, int ncols);

    virtual RealMatrix _matrix(int nrows, int cols) const = 0;

private:
    void reset();

protected:
    int _n_rows;

    int _n_cols;

    int _halfCols;

    // used directly with FFTW3
    fftw_plan _forwardPlan;

    fftw_plan _backwardPlan;

    double* _realData;

    fftw_complex* _transformedData;

    std::vector<std::complex<double>> _transformedKernel;
};

} // end namespace nsx

#endif // CORE_SEARCH_PEAKS_ATOMICCONVOLVER_H
