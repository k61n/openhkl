//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/AtomicConvolver.h
//! @brief     Defines class AtomicConvolver
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_ATOMICCONVOLVER_H
#define OHKL_CORE_CONVOLVE_ATOMICCONVOLVER_H

#include "core/convolve/Convolver.h" // inherits from

#include <fftw3.h>
#include <vector>

namespace ohkl {

//! Pure virtual base class for ConstantConvolver, DeltaConvolver, RadialConvolver.

class AtomicConvolver : public Convolver {
 public:
    AtomicConvolver();

    AtomicConvolver(const std::map<std::string, double>& parameters);

    AtomicConvolver(const AtomicConvolver& other) = default;

    AtomicConvolver& operator=(const AtomicConvolver& other) = default;

    virtual ~AtomicConvolver() = 0;

    RealMatrix matrix(int nrows, int ncols) const;

    //! Convolve an image
    virtual RealMatrix convolve(const RealMatrix& image) override;

 protected:
    void updateKernel(int nrows, int ncols);

    virtual RealMatrix _matrix(int nrows, int cols) const = 0;

 private:
    void reset();

 protected:
    int _n_rows{0};
    int _n_cols{0};
    int _halfCols;

    fftw_plan _forwardPlan;
    fftw_plan _backwardPlan;

    double* _realData{nullptr};
    fftw_complex* _transformedData;

    std::vector<std::complex<double>> _transformedKernel;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_ATOMICCONVOLVER_H
