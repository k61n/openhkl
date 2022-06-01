//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/GonioFit.h
//! @brief     Defines struct GonioFit and declares functions fit...GonioOffset
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_ALGO_GONIOFIT_H
#define NSX_CORE_ALGO_GONIOFIT_H

#include "core/data/DataTypes.h"
#include "core/gonio/Gonio.h"
#include <cstddef>
#include <vector>

namespace nsx {

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

} // namespace nsx

#endif // NSX_CORE_ALGO_GONIOFIT_H
