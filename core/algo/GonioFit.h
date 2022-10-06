//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/GonioFit.h
//! @brief     Defines struct GonioFit and declares functions fit...GonioOffset
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_ALGO_GONIOFIT_H
#define OHKL_CORE_ALGO_GONIOFIT_H

#include "core/data/DataTypes.h"
#include "core/gonio/Gonio.h"
#include <cstddef>
#include <vector>

namespace ohkl {

//! Stores the result of fitting parameters of a Gonio.

struct GonioFit {
    bool success;
    std::vector<double> offsets;
    std::vector<double> cost_function;
};

GonioFit fitDetectorGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance);
GonioFit fitSampleGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance);

} // namespace ohkl

#endif // OHKL_CORE_ALGO_GONIOFIT_H
