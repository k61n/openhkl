//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/RadialConvolver.cpp
//! @brief     Implements class RadialConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/RadialConvolver.h"

namespace ohkl {

RadialConvolver::RadialConvolver() : AtomicConvolver({{"r_in", 5}, {"r_out", 10}})
{
    _convolver_type = "radial";
}

Convolver* RadialConvolver::clone() const
{
    return new RadialConvolver(*this);
}

RadialConvolver::RadialConvolver(const std::map<std::string, double>& parameters)
    : RadialConvolver()
{
    setParameters(parameters);
}

std::pair<size_t, size_t> RadialConvolver::kernelSize() const
{
    size_t r = _parameters.at("r_out");

    return std::make_pair(r, r);
}

RealMatrix RadialConvolver::_matrix(int nrows, int ncols) const
{
    const double r_in = _parameters.at("r_in");
    const double r_out = _parameters.at("r_out");

    // sanity checks
    if (r_in < 0 || r_out < r_in)
        throw std::runtime_error("Radial convolver called with invalid parameters");

    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);

    for (int i = 0; i < nrows; ++i) {
        for (int j = 0; j < ncols; ++j) {
            // shift so that (0,0) = (rows, cols) = (rows, 0) = (0, cols) is the
            // center of the kernel
            double x = j > ncols / 2 ? ncols - j : j;
            double y = i > nrows / 2 ? nrows - i : i;

            double dist2 = x * x + y * y;

            if (dist2 >= r_in * r_in && dist2 < r_out * r_out)
                kernel(i, j) = 1.0;
        }
    }

    kernel /= kernel.sum();

    return kernel;
}

} // namespace ohkl
