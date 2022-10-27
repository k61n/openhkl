//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/GradientConvolver.h
//! @brief     Defines class GradientConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_GRADIENTCONVOLVER_H
#define OHKL_CORE_CONVOLVE_GRADIENTCONVOLVER_H

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Gradient computing kernel
class GradientConvolver : public AtomicConvolver {
 public:
    GradientConvolver();

    GradientConvolver(const GradientConvolver& other) = default;

    GradientConvolver(const std::map<std::string, double>& parameters);

    ~GradientConvolver() = default;

    GradientConvolver& operator=(const GradientConvolver& other) = default;

    Convolver* clone() const override;

    std::pair<size_t, size_t> kernelSize() const override;

 protected:
    void setOperator(const Eigen::MatrixXd& matrix_x, const Eigen::MatrixXd& matrix_y);

    RealMatrix _matrix(int nrows, int ncols) const override;

 private:
    //! Operator for gradient in x direction
    Eigen::MatrixXd _operator_x;
    //! Operator for gradient in y direction
    Eigen::MatrixXd _operator_y;
    //! Size of matrix
    std::size_t _dim;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_GRADIENTCONVOLVER_H
