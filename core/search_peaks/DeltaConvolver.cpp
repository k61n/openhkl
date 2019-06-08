//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/search_peaks/DeltaConvolver.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/search_peaks/DeltaConvolver.h"

namespace nsx {

DeltaConvolver::DeltaConvolver() : AtomicConvolver() {}

DeltaConvolver::DeltaConvolver(const std::map<std::string, double>& parameters) : DeltaConvolver()
{
    setParameters(parameters);
}

Convolver* DeltaConvolver::clone() const
{
    return new DeltaConvolver(*this);
}

std::pair<size_t, size_t> DeltaConvolver::kernelSize() const
{
    return std::make_pair(0, 0);
}

RealMatrix DeltaConvolver::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    kernel(0, 0) = 1.0;

    return kernel;
}

} // end namespace nsx
