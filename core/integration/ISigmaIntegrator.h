//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/ISigmaIntegrator.h
//! @brief     Defines class ISigmaIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INTEGRATION_ISIGMAINTEGRATOR_H
#define CORE_INTEGRATION_ISIGMAINTEGRATOR_H

#include "core/integration/PixelSumIntegrator.h"
#include "core/shape/ShapeLibrary.h"

namespace nsx {

//! Compute the integrated intensity via the I/sigma method used in RETREAT.

class ISigmaIntegrator : public PixelSumIntegrator {
 public:
    ISigmaIntegrator();
    bool compute(
       Peak3D* peak, ShapeLibrary* shape_library, 
       const IntegrationRegion& region) override;

};

} // namespace nsx

#endif // CORE_INTEGRATION_ISIGMAINTEGRATOR_H
