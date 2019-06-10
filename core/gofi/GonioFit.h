//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/gofi/GonioFit.h
//! @brief     Defines struct GonioFit
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_GOFI_GONIOFIT_H
#define CORE_GOFI_GONIOFIT_H

#include <vector>

namespace nsx {

//! Helper struct for storing the result of a gonio fit
struct GonioFit {
    bool success;
    std::vector<double> offsets;
    std::vector<double> cost_function;
};

} // namespace nsx

#endif // CORE_GOFI_GONIOFIT_H
