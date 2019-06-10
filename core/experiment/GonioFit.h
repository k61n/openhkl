//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/GonioFit.h
//! @brief     Defines struct GonioFit and declares functions fit...GonioOffset
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_GOFI_GONIOFIT_H
#define CORE_GOFI_GONIOFIT_H

#include "core/experiment/DataTypes.h"
#include "core/gonio/Gonio.h"
#include <cstddef>
#include <vector>

namespace nsx {

//! Helper struct for storing the result of a gonio fit
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

#endif // CORE_GOFI_GONIOFIT_H
