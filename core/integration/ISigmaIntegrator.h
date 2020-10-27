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

#ifndef NSX_CORE_INTEGRATION_ISIGMAINTEGRATOR_H
#define NSX_CORE_INTEGRATION_ISIGMAINTEGRATOR_H

#include "core/integration/PixelSumIntegrator.h"
#include "core/shape/ShapeCollection.h"

namespace nsx {

//! Compute the integrated intensity via the I/sigma method used in RETREAT.

class ISigmaIntegrator : public PixelSumIntegrator {
 public:
    ISigmaIntegrator();
    bool compute(
        Peak3D* peak, ShapeCollection* shape_collection, const IntegrationRegion& region) override;
};

} // namespace nsx

#endif // NSX_CORE_INTEGRATION_ISIGMAINTEGRATOR_H
