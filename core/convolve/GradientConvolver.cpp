//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/GradientConvolver.cpp
//! @brief     Implements class GradientConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/GradientConvolver.h"
#include "core/convolve/AtomicConvolver.h"

#include <iostream>

namespace ohkl {

GradientConvolver::GradientConvolver() : AtomicConvolver() { }

GradientConvolver::GradientConvolver(const std::map<std::string, double>& parameters)
    : AtomicConvolver(parameters)
{
    _norm_fac = 1.0 / 8.0;
}

Convolver* GradientConvolver::clone() const
{
    return new GradientConvolver(*this);
}

std::pair<size_t, size_t> GradientConvolver::kernelSize() const
{
    return std::make_pair(_dim, _dim);
}

void GradientConvolver::setOperator(const Eigen::MatrixXd& matrix_x, const Eigen::MatrixXd& matrix_y)
{
    _operator_x = matrix_x;
    _operator_y = matrix_y;
    _dim = matrix_x.rows(); // assume the matrix is square
}

RealMatrix GradientConvolver::_matrix(int nrows, int ncols) const
{
    Eigen::MatrixXd matrix;
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    if (_parameters.find("x") != _parameters.end()) {
        matrix = _operator_x;
    } else if (_parameters.find("y") != _parameters.end()) {
        matrix = _operator_y;
    } else {
        throw std::runtime_error("Gradient convolver missing direction parameter (x/y)");
    }

    const int half = _dim / 2;
    // offset = 0 if odd dimension, 1 if even dimension
    const int offset = _dim % 2 ? 0 : 1;

    int k, l, row, col;

    for (int i = 0; i < _dim; ++i) {
        k = -half + offset + i;
        row = (k + nrows - offset) % nrows;
        for (int j = 0; j < _dim; ++j) {
            l = -half + offset + j;
            col = (l + ncols - offset) % ncols;
            kernel(row, col) = matrix(j, i);
        }
    }

    return kernel * _norm_fac;
}

} // namespace ohkl
