//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/ConstantConvolver.cpp
//! @brief     Implements class ConstantConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/ConstantConvolver.h"

namespace ohkl {

ConstantConvolver::ConstantConvolver() : AtomicConvolver({{"box_size", 3}}) { }

ConstantConvolver::ConstantConvolver(const std::map<std::string, double>& parameters)
    : ConstantConvolver()
{
    _convolver_type = "constant";
    setParameters(parameters);
}

Convolver* ConstantConvolver::clone() const
{
    return new ConstantConvolver(*this);
}

std::pair<size_t, size_t> ConstantConvolver::kernelSize() const
{
    size_t r = _parameters.at("box_size");

    return std::make_pair(r, r);
}

RealMatrix ConstantConvolver::_matrix(int nrows, int ncols) const
{
    const int box_size = static_cast<int>(_parameters.at("box_size"));

    // sanity checks
    if (box_size <= 0)
        throw std::runtime_error("Constant kernel called with invalid parameters");

    const double norm_factor = static_cast<double>(box_size * box_size);

    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);

    const int half = box_size / 2;
    const int offset = box_size % 2 ? 0 : 1;
    for (int i = -half + offset; i <= half; ++i) {
        const int row = (i + nrows - offset) % nrows;
        for (int j = -half + offset; j <= half; ++j) {
            const int col = (j + ncols - offset) % ncols;
            kernel(row, col) = 1.0 / norm_factor;
        }
    }

    return kernel;
}

} // namespace ohkl
