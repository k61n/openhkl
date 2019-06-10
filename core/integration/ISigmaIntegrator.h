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

#include "core/integration/ShapeLibrary.h"
#include "core/integration/PixelSumIntegrator.h"

namespace nsx {

//! Compute the integrated intensity via the I/sigma method used in RETREAT.
class ISigmaIntegrator : public PixelSumIntegrator {
public:
    ISigmaIntegrator(sptrShapeLibrary library, double radius, double nframes);
    bool compute(sptrPeak3D peak, const IntegrationRegion& region) override;

private:
    sptrShapeLibrary _library;
    double _radius;
    double _nframes;
};

} // namespace nsx

#endif // CORE_INTEGRATION_ISIGMAINTEGRATOR_H
